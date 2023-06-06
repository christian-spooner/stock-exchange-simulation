using System.Net.Sockets;
using System.Text;
using Newtonsoft.Json;
using Npgsql;
using Utilities;

namespace Clearinghouse
{
    public class SettlementBank
    {
        private readonly string connectionString;

        public SettlementBank(string connectionString)
        {
            this.connectionString = connectionString;
        }

        public Result<decimal> GetBalance(string clientId)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;
                    command.CommandText = "SELECT balance FROM account_balances WHERE client_id = @clientId";
                    command.Parameters.AddWithValue("clientId", clientId);

                    var balance = command.ExecuteScalar();

                    if (balance != null && balance != DBNull.Value)
                    {
                        return Result.Success(Convert.ToDecimal(balance));
                    }

                    return Result.Success(0.00m);
                }
            }
        }

        public Result<int> Deposit(string clientId, decimal amount)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;
                    command.CommandText = "INSERT INTO account_balances (client_id, balance) VALUES (@clientId, @amount) " +
                                        "ON CONFLICT (client_id) DO UPDATE SET balance = account_balances.balance + @amount";
                    command.Parameters.AddWithValue("clientId", clientId);
                    command.Parameters.AddWithValue("amount", amount);

                    command.ExecuteNonQuery();
                }
            }

            return Result.Success(0);
        }

        public Result<int> Withdraw(string clientId, decimal amount)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;
                    command.CommandText = "UPDATE account_balances SET balance = balance - @amount WHERE client_id = @clientId AND balance >= @amount";
                    command.Parameters.AddWithValue("clientId", clientId);
                    command.Parameters.AddWithValue("amount", amount);

                    var rowsAffected = command.ExecuteNonQuery();

                    if (rowsAffected == 0)
                    {
                        return Result.Failure<int>("Insufficient balance");
                    }
                }
            }

            return Result.Success(0);
        }

        public Result<int> Transfer(string fromClientId, string toClientId, decimal amount)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;

                    command.CommandText = "UPDATE account_balances SET balance = balance - @amount WHERE client_id = @fromClientId AND balance >= @amount";
                    command.Parameters.AddWithValue("fromClientId", fromClientId);
                    command.Parameters.AddWithValue("amount", amount);

                    var rowsAffected = command.ExecuteNonQuery();

                    if (rowsAffected == 0)
                    {
                        return Result.Failure<int>("Insufficient balance");
                    }

                    command.CommandText = "INSERT INTO account_balances (client_id, balance) VALUES (@toClientId, @amount) " +
                                        "ON CONFLICT (client_id) DO UPDATE SET balance = account_balances.balance + @amount";
                    command.Parameters.AddWithValue("toClientId", toClientId);

                    command.ExecuteNonQuery();
                }
            }

            return Result.Success(0);
        }


        public Result<string> HandleMessage(string message)
        {
            var settlementCommand = JsonConvert.DeserializeObject<Dictionary<string, object>>(message);
            if (settlementCommand == null || !settlementCommand.ContainsKey("type") || !settlementCommand.ContainsKey("client_id"))
            {
                Logger.LogInfo("Invalid message received. Missing required fields.");
                return Result.Failure<String>("Missing fields");
            }
            else
            {
                string operation = (string)settlementCommand["type"];
                string clientId = (string)settlementCommand["client_id"];

                if (operation == "balance")
                {
                    Result<decimal> result = GetBalance(clientId);
                    if (result.IsError)
                    {
                        Result.Failure<string>(result.Error!);
                    } 
                    return Result.Success(result.Value.ToString());
                }

                if (!settlementCommand.ContainsKey("amount"))
                {
                    Logger.LogInfo("Invalid message received. Missing amount field.");
                    return Result.Failure<String>("Missing fields");
                }

                decimal amount = Convert.ToDecimal(settlementCommand["amount"]);

                if (operation == "deposit")
                {
                    Result<int> result = Deposit(clientId, amount);
                    if (result.IsError)
                    {
                        return Result.Failure<String>(result.Error!);
                    }
                }
                else if (operation == "withdraw")
                {
                    Result<int> result = Withdraw(clientId, amount);
                    if (result.IsError)
                    {
                        return Result.Failure<String>(result.Error!);
                    }
                }
                else if (operation == "transfer")
                {
                    string toClientId = (string)settlementCommand["to_client_id"];
                    Result<int> result = Transfer(clientId, toClientId, amount);
                    if (result.IsError)
                    {
                        return Result.Failure<String>(result.Error!);
                    }
                }
                else
                {
                    Logger.LogInfo("Invalid message received. Operation must be 'deposit', 'withdraw', or 'transfer'.");
                }

                return Result.Success(GetBalance(clientId).Value.ToString());
            }
        }

        public void Run()
        {
            string host = "127.0.0.1";
            int port = 2403;

            var tcpListener = new TcpListener(System.Net.IPAddress.Parse(host), port);
            tcpListener.Start();
            Console.WriteLine($"Settlement listening on port {port}");

            while (true)
            {
                var tcpClient = tcpListener.AcceptTcpClient();

                using (var stream = tcpClient.GetStream())
                {
                    try
                    {
                        byte[] lengthBuffer = new byte[4];
                        stream.Read(lengthBuffer, 0, 4);
                        int messageLength = BitConverter.ToInt32(lengthBuffer, 0);

                        byte[] messageBuffer = new byte[messageLength];
                        stream.Read(messageBuffer, 0, messageLength);
                        string message = Encoding.UTF8.GetString(messageBuffer);

                        Result<string> result = HandleMessage(message);
                        object responseObject;
                        if (result.IsError)
                        {
                            responseObject = new { error = result.Error };
                        }
                        else
                        {
                            responseObject = new { balance = result.Value };
                        }
                        string responseJson = Newtonsoft.Json.JsonConvert.SerializeObject(responseObject);
                        byte[] responseBytes = Encoding.UTF8.GetBytes(responseJson);
                        stream.Write(responseBytes, 0, responseBytes.Length);
                    }
                    catch (IOException)
                    {
                        Logger.LogInfo($"Connection closed by {tcpClient.Client.RemoteEndPoint}");
                    }
                    catch (Exception ex)
                    {
                        Logger.LogError(ex.ToString());
                    }
                }
            }
        }
    }

}
