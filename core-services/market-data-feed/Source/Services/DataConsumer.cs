using Microsoft.Extensions.Hosting;
using RabbitMQ.Client;
using RabbitMQ.Client.Events;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace MarketDataFeed
{
    public class DataConsumerService : BackgroundService
    {
        private readonly MessageHandler _messageHandler;

        public DataConsumerService(MessageHandler messageHandler)
        {
            _messageHandler = messageHandler;
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            var factory = new ConnectionFactory() { HostName = "rabbitmq" };

            using (var connection = factory.CreateConnection())
            using (var channel = connection.CreateModel())
            {
                channel.QueueDeclare(queue: "data_feed",
                                    durable: true,
                                    exclusive: false,
                                    autoDelete: false,
                                    arguments: null);

                var consumer = new EventingBasicConsumer(channel);
                consumer.Received += async (model, ea) =>
                {
                    var body = ea.Body.ToArray();
                    var message = Encoding.UTF8.GetString(body);
                    Logger.LogInfo($"Received message: {message}");
                    await _messageHandler.HandleIncomingMessageAsync(message, stoppingToken);
                };

                channel.BasicConsume(queue: "data_feed",
                                    autoAck: true,
                                    consumer: consumer);

                while (!stoppingToken.IsCancellationRequested)
                {
                    await Task.Delay(1000, stoppingToken);
                }
            }
        }
    }
}
