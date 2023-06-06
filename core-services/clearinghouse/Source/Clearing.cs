using System;
using System.Net.Sockets;
using System.Text;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using RabbitMQ.Client;
using Utilities;

namespace Clearinghouse
{
    public class Clearing
    {
        private Depository depository;

        public Clearing(Depository depository)
        {
            this.depository = depository;
        }

        public Result<Trade> Clear(ref Trade trade)
        {
            // TODO: netting
            return Result.Success(trade);
        }

        public Result<Trade> Settle(ref Trade trade, bool isCleared)
        {
            if (trade.Side == "buy")
            {
                Result<int> deliveryResult = this.depository.Transfer("DTCC", trade.ClientId, trade.SecurityId, trade.Quantity);
                if (deliveryResult.IsError)
                {
                    return Result.Failure<Trade>(deliveryResult.Error!);
                }
                Result<string> paymentResult = SendTransferRequest(trade.ClientId, "DTCC", trade.Quantity * trade.Price);
                if (paymentResult.IsError)
                {
                    this.depository.Transfer(trade.ClientId, "DTCC", trade.SecurityId, trade.Quantity);
                    return Result.Failure<Trade>(paymentResult.Error!);
                }
            } else if (trade.Side == "sell") {
                Result<int> deliveryResult = this.depository.Transfer(trade.ClientId, "DTCC", trade.SecurityId, trade.Quantity);
                if (deliveryResult.IsError)
                {
                    return Result.Failure<Trade>(deliveryResult.Error!);
                }
                Result<string> paymentResult = SendTransferRequest("DTCC", trade.ClientId, trade.Quantity * trade.Price);
                if (paymentResult.IsError)
                {
                    this.depository.Transfer("DTCC", trade.ClientId, trade.SecurityId, trade.Quantity);
                    return Result.Failure<Trade>(paymentResult.Error!);
                }
            }
            else
            {
                Logger.LogInfo("Received invalid settlement request");
                return Result.Failure<Trade>("Invalid side");
            }

            return Result.Success(trade);
        }

        public void HandleTrade(ref Trade trade)
        {
            Result<Trade> clearingResult = Clear(ref trade);
            if (clearingResult.IsError)
            {
                Logger.LogInfo($"Failed clearing: {clearingResult.Error}");
                SendReport(trade, false, clearingResult.Error);
                return;
            }

            Result<Trade> settlementResult = Settle(ref trade, true);
            if (settlementResult.IsError)
            {   
                Logger.LogInfo($"Failed settlement: {settlementResult.Error}");
                SendReport(trade, false, settlementResult.Error);
                return;
            }

            SendReport(trade, true);
        }

        public Result<string> SendTransferRequest(string fromClientId, string toClientId, decimal amount)
        {
            var transferRequest = new Dictionary<string, object>
            {
                { "type", "transfer" },
                { "client_id", fromClientId },
                { "to_client_id", toClientId },
                { "amount", amount }
            };
            var message = JsonConvert.SerializeObject(transferRequest);

            string host = "127.0.0.1";
            int port = 2403;
            using (var client = new TcpClient(host, port))
            using (var stream = client.GetStream())
            {
                byte[] lengthBuffer = BitConverter.GetBytes(message.Length);
                stream.Write(lengthBuffer, 0, lengthBuffer.Length);

                byte[] messageBuffer = Encoding.UTF8.GetBytes(message);
                stream.Write(messageBuffer, 0, messageBuffer.Length);

                byte[] responseBytes = new byte[4096];
                int bytesRead = stream.Read(responseBytes, 0, responseBytes.Length);
                string responseJson = Encoding.UTF8.GetString(responseBytes, 0, bytesRead);
                var responseObject = Newtonsoft.Json.JsonConvert.DeserializeObject<JObject>(responseJson);

                if (responseObject != null && responseObject.Property("balance") != null)
                {
                    string balance = responseObject["balance"]!.Value<string>()!;
                    Logger.LogInfo($"Transfer request completed. New balance for {fromClientId}: {balance}");
                    return Result.Success(balance);
                }
                else if (responseObject != null && responseObject.Property("error") != null)
                {
                    string error = responseObject["error"]!.Value<string>()!;
                    return Result.Failure<String>(error);
                }
                else
                {
                    Logger.LogError($"Transfer request: null response.");
                    return Result.Failure<String>("Null response");
                }
            }
        }

        public void SendReport(Trade trade, bool isSettled, string? reason = null)
        {
            Report report;

            if (isSettled)
            {
                report = new Report
                {
                    event_type = "settlement",
                    status = "confirmed",
                    order_id = trade.OrderId,
                    client_id = trade.ClientId,
                    security_id = trade.SecurityId,
                    side = trade.Side,
                    quantity = trade.Quantity,
                    price = trade.Price,
                    reason = null
                };
            }
            else
            {
                report = new Report
                {
                    event_type = "settlement",
                    status = "denied",
                    order_id = trade.OrderId,
                    client_id = trade.ClientId,
                    security_id = trade.SecurityId,
                    side = trade.Side,
                    quantity = trade.Quantity,
                    price = trade.Price,
                    reason = reason
                };
            }

            var message = new
            {
                message_type = "report",
                body = report
            };

            string messageJson = JsonConvert.SerializeObject(message);

            using (var connection = new ConnectionFactory() { HostName = "rabbitmq" }.CreateConnection())
            using (var channel = connection.CreateModel())
            {
                channel.QueueDeclare(queue: "data_feed", durable: true, exclusive: false, autoDelete: false, arguments: null);

                byte[] body = Encoding.UTF8.GetBytes(messageJson);
                channel.BasicPublish(exchange: "stock_exchange", routingKey: "data_feed", basicProperties: null, body: body);
            }
        }

        public void Run()
        {
            string host = "127.0.0.1";
            int port = 2402;

            var tcpListener = new TcpListener(System.Net.IPAddress.Parse(host), port);
            tcpListener.Start();
            Console.WriteLine($"Clearing listening on port {port}");

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
                        var trade = JsonConvert.DeserializeObject<Trade>(message);

                        byte[] responseBytes = System.Text.Encoding.UTF8.GetBytes("received");
                        stream.Write(responseBytes, 0, responseBytes.Length);
                        HandleTrade(ref trade);
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