using System;
using System.Collections.Generic;
using Npgsql;
using Utilities;

namespace Clearinghouse
{
    public class Depository
    {
        private readonly string connectionString;

        public Depository(string connectionString)
        {
            this.connectionString = connectionString;
        }

        public Result<int> Deposit(string clientId, string securityId, int quantity)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;
                    command.CommandText = "SELECT quantity FROM securities_deposit WHERE client_id = @clientId AND security_id = @securityId";
                    command.Parameters.AddWithValue("clientId", clientId);
                    command.Parameters.AddWithValue("securityId", securityId);

                    object? result = command.ExecuteScalar();

                    if (result == null)
                    {
                        command.Parameters.Clear();
                        command.CommandText = "INSERT INTO securities_deposit (client_id, security_id, quantity) VALUES (@clientId, @securityId, @quantity)";
                        command.Parameters.AddWithValue("clientId", clientId);
                        command.Parameters.AddWithValue("securityId", securityId);
                        command.Parameters.AddWithValue("quantity", quantity);
                    }
                    else
                    {
                        int currentQuantity = Convert.ToInt32(result);
                        int newQuantity = currentQuantity + quantity;
                        command.Parameters.Clear();
                        command.CommandText = "UPDATE securities_deposit SET quantity = @newQuantity WHERE client_id = @clientId AND security_id = @securityId";
                        command.Parameters.AddWithValue("clientId", clientId);
                        command.Parameters.AddWithValue("securityId", securityId);
                        command.Parameters.AddWithValue("newQuantity", newQuantity);
                    }

                    command.ExecuteNonQuery();
                }
            }

            return Result.Success<int>(0);
        }

        public Result<int> Withdraw(string clientId, string securityId, int quantity)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;
                    command.CommandText = "UPDATE securities_deposit SET quantity = quantity - @quantity WHERE client_id = @clientId AND security_id = @securityId AND quantity >= @quantity";
                    command.Parameters.AddWithValue("clientId", clientId);
                    command.Parameters.AddWithValue("securityId", securityId);
                    command.Parameters.AddWithValue("quantity", quantity);

                    var rowsAffected = command.ExecuteNonQuery();

                    if (rowsAffected == 0)
                    {
                        return Result.Failure<int>("Insufficient holdings");
                    }
                }
            }

            return Result.Success(0);
        }

        public Result<int> Transfer(string fromClientId, string toClientId, string securityId, int transferQuantity)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;
                    command.CommandText = "SELECT quantity FROM securities_deposit WHERE client_id = @clientId AND security_id = @securityId";
                    command.Parameters.AddWithValue("clientId", fromClientId);
                    command.Parameters.AddWithValue("securityId", securityId);

                    var currentQuantity = (int?)command.ExecuteScalar();

                    if (currentQuantity == null || currentQuantity < transferQuantity)
                    {
                        return Result.Failure<int>("Insufficient holdings");
                    }

                    Withdraw(fromClientId, securityId, transferQuantity);

                    Deposit(toClientId, securityId, transferQuantity);

                    return Result.Success(0);
                }
            }
        }

        public Result<int> GetHoldings(string securityId, string clientId)
        {
            using (var connection = new NpgsqlConnection(connectionString))
            {
                connection.Open();

                using (var command = new NpgsqlCommand())
                {
                    command.Connection = connection;
                    command.CommandText = "SELECT quantity FROM securities_deposit WHERE client_id = @clientId AND security_id = @securityId";
                    command.Parameters.AddWithValue("clientId", clientId);

                    using (var reader = command.ExecuteReader())
                    {
                        while (reader.Read())
                        {
                            var quantity = reader.GetInt32(0);
                            return Result.Success(quantity);
                        }
                    }
                }
            }

            return Result.Failure<int>("Failed to retrieve holdings");
        }
    }
}
