using RabbitMQ.Client;
using RabbitMQ.Client.Events;
using System;
using System.Threading.Tasks;
using System.Net.Sockets;
using System.Text;
using Clearinghouse;
using Npgsql;

class Program
{
    private static ManualResetEvent ExitSignal = new ManualResetEvent(false);

    static void Main(string[] args)
    {
        var connectionString = new NpgsqlConnectionStringBuilder
        {
            Host = "db",
            Port = 5432,
            Database = "clearinghouse",
            Username = "postgres",
            Password = "postgres"
        }.ToString();

        Depository depository = new Depository(connectionString);
        Task.Run(() => new Clearing(depository).Run());
        Task.Run(() => new SettlementBank(connectionString).Run());

        var factory = new ConnectionFactory() { HostName = "rabbitmq" };
        using (var connection = factory.CreateConnection())
        using (var channel = connection.CreateModel())
        {
            channel.QueueDeclare(queue: "order_execution",
                                 durable: true,
                                 exclusive: false,
                                 autoDelete: false,
                                 arguments: null);

            var consumer = new EventingBasicConsumer(channel);
            consumer.Received += (model, ea) =>
            {
                var body = ea.Body.ToArray();
                var message = Encoding.UTF8.GetString(body);
                HandleMessage(message);
            };
            channel.BasicConsume(queue: "order_execution",
                                 autoAck: true,
                                 consumer: consumer);

            ExitSignal.WaitOne();
        }
    }

    static void HandleMessage(string message)
    {
        Logger.LogInfo($"Received message: {message}");

        using (var client = new TcpClient("127.0.0.1", 2402))
        {
            var stream = client.GetStream();

            byte[] body = Encoding.UTF8.GetBytes(message);
            byte[] lengthBuffer = BitConverter.GetBytes(body.Length);
            stream.Write(lengthBuffer, 0, 4);

            stream.Write(body, 0, body.Length);

            byte[] responseBytes = new byte[1024];
            int bytesRead = stream.Read(responseBytes, 0, responseBytes.Length);
            string response = Encoding.UTF8.GetString(responseBytes, 0, bytesRead);
        }
    }
}
