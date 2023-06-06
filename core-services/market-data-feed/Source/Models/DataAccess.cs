using System;
using System.Data;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Npgsql;

namespace MarketDataFeed
{
    public class DataAccess
    {
        private static string MDconnectionString = "Host=db;Database=market_data;Username=postgres;Password=postgres";
        private static string CHconnectionString = "Host=db;Database=clearinghouse;Username=postgres;Password=postgres";

        public static string GetLatestReport(string securityId, string id_str)
        {
            if (!int.TryParse(id_str, out int order_id))
            {
                var error = new { message = "invalid orderId, must be integer" };
                return JsonConvert.SerializeObject(error);
            }

            using (var localConnection = new NpgsqlConnection(MDconnectionString))
            {
                localConnection.Open();

                using (var cmd = new NpgsqlCommand("SELECT * FROM reports WHERE security_id=@security_id AND order_id=@order_id ORDER BY id DESC LIMIT 1", localConnection))
                {
                    cmd.Parameters.AddWithValue("security_id", securityId);
                    cmd.Parameters.AddWithValue("order_id", order_id);

                    using (var reader = cmd.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            var report = new
                            {
                                event_type = reader.GetString(1),
                                status = reader.GetString(2),
                                order_id = reader.GetInt32(3),
                                client_id = reader.GetString(4),
                                security_id = reader.GetString(5),
                                side = reader.GetString(6),
                                quantity = reader.GetInt32(7),
                                price = reader.GetDecimal(8),
                                reason = reader.IsDBNull(9) ? (string?)null : reader.GetString(9),
                            };
                            return JsonConvert.SerializeObject(report) + "\n";
                        }
                        else
                        {
                            var error = new { message = "order not found" };
                            return JsonConvert.SerializeObject(error) + "\n";
                        }
                    }
                }
            }
        }

        public static string GetLatestStatus(string securityId, string id_str)
        {
            if (!int.TryParse(id_str, out int order_id))
            {
                var error = new { message = "invalid orderId, must be integer" };
                return JsonConvert.SerializeObject(error);
            }

            using (var localConnection = new NpgsqlConnection(MDconnectionString))
            {
                localConnection.Open();

                using (var cmd = new NpgsqlCommand("SELECT * FROM reports WHERE security_id=@security_id AND order_id=@order_id ORDER BY id DESC LIMIT 1", localConnection))
                {
                    cmd.Parameters.AddWithValue("security_id", securityId);
                    cmd.Parameters.AddWithValue("order_id", order_id);

                    using (var reader = cmd.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            string event_type = reader.GetString(1);
                            string status = reader.GetString(2);
                            string response = status switch
                            {
                                "confirmed" => event_type switch
                                {
                                    "order" => "new",
                                    "trade" => "filled",
                                    _ => "settled"
                                },
                                "denied" => event_type switch
                                {
                                    "order" => "rejected",
                                    "trade" => "cancelled",
                                    _ => "failed"
                                },
                                _ => ((Func<string>)(() =>
                                {
                                    Logger.LogInfo("Invalid status fetch.");
                                    return "unknown";
                                }))()
                            };
                            var responseObj = new { status = response };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                        else
                        {
                            var responseObj = new { message = "order not found" };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                    }
                }
            }
        }

        public static string GetLatestPrice(string securityId)
        {
            using (var localConnection = new NpgsqlConnection(MDconnectionString))
            {
                localConnection.Open();

                using (var cmd = new NpgsqlCommand("SELECT bid, ask FROM prices WHERE security_id=@security_id ORDER BY id DESC LIMIT 1", localConnection))
                {
                    cmd.Parameters.AddWithValue("security_id", securityId);

                    using (var reader = cmd.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            decimal bid = reader.GetDecimal(0);
                            decimal ask = reader.GetDecimal(1);
                            var responseObj = new { bid = bid, ask = ask };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                        else
                        {
                            var responseObj = new { message = $"price not found for security id: {securityId}" };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                    }
                }
            }
        }

        public static string GetBalance(string clientId)
        {
            using (var localConnection = new NpgsqlConnection(CHconnectionString))
            {
                localConnection.Open();

                using (var cmd = new NpgsqlCommand("SELECT balance FROM account_balances WHERE client_id=@client_id", localConnection))
                {
                    cmd.Parameters.AddWithValue("client_id", clientId);

                    using (var reader = cmd.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            decimal balance = reader.GetDecimal(0);
                            var responseObj = new { balance = balance };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                        else
                        {
                            var responseObj = new { message = $"balance not found for client id: {clientId}" };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                    }
                }
            }
        }

        public static string GetHoldings(string securityId, string clientId)
        {
            using (var localConnection = new NpgsqlConnection(CHconnectionString))
            {
                localConnection.Open();

                using (var cmd = new NpgsqlCommand("SELECT quantity FROM securities_deposit WHERE security_id=@security_id AND client_id=@client_id", localConnection))
                {
                    cmd.Parameters.AddWithValue("security_id", securityId);
                    cmd.Parameters.AddWithValue("client_id", clientId);

                    using (var reader = cmd.ExecuteReader())
                    {
                        if (reader.Read())
                        {
                            int shares = reader.GetInt32(0);
                            var responseObj = new { shares = shares };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                        else
                        {
                            var responseObj = new { message = $"holdings not found for client id: {clientId}" };
                            return JsonConvert.SerializeObject(responseObj) + "\n";
                        }
                    }
                }
            }
        }
    }
}