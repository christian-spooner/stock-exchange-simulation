using System;
using System.Data;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Npgsql;

namespace MarketDataFeed
{
    public class DataInput
    {
        private static string connectionString = "Host=db;Database=market_data;Username=postgres;Password=postgres";
        private static NpgsqlConnection connection = new NpgsqlConnection(connectionString);

        public static void StoreReport(Report report)
        {
            string stmt = @"
                INSERT INTO reports (event, status, order_id, client_id, security_id, side, quantity, price, reason)
                VALUES (@EventType, @Status, @OrderId, @ClientId, @SecurityId, @Side, @Quantity, @Price, @Reason)
            ";

            using (var cmd = new NpgsqlCommand(stmt, connection))
            {
                cmd.Parameters.AddWithValue("EventType", report.EventType);
                cmd.Parameters.AddWithValue("Status", report.Status);
                cmd.Parameters.AddWithValue("OrderId", report.OrderId);
                cmd.Parameters.AddWithValue("ClientId", report.ClientId);
                cmd.Parameters.AddWithValue("SecurityId", report.SecurityId);
                cmd.Parameters.AddWithValue("Side", report.Side);
                cmd.Parameters.AddWithValue("Quantity", report.Quantity);
                cmd.Parameters.AddWithValue("Price", report.Price);

                if (report.Reason != null)
                {
                    cmd.Parameters.AddWithValue("Reason", report.Reason);
                }
                else
                {
                    cmd.Parameters.AddWithValue("Reason", DBNull.Value);
                }

                connection.Open();
                cmd.ExecuteNonQuery();
                connection.Close();
            }
        }

        public static void StorePrice(PriceData priceData)
        {
            string stmt = @"
            INSERT INTO prices (security_id, bid, ask)
            VALUES (@SecurityId, @Bid, @Ask)
        ";

            using (var cmd = new NpgsqlCommand(stmt, connection))
            {
                cmd.Parameters.AddWithValue("SecurityId", priceData.SecurityId);
                cmd.Parameters.AddWithValue("Bid", priceData.Bid);
                cmd.Parameters.AddWithValue("Ask", priceData.Ask);

                connection.Open();
                cmd.ExecuteNonQuery();
                connection.Close();
            }
        }
    }
}