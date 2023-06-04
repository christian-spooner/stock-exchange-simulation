using Microsoft.AspNetCore.SignalR;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Linq;
using System;

namespace MarketDataFeed
{
    public class Message
    {
        [JsonProperty("message_type")]
        public string? MessageType { get; set; }

        [JsonProperty("body")]
        public JObject? Body { get; set; }
    }

    public class MessageHandler
    {
        private readonly IMessageBroadcaster _webSocketMiddleware;

        public MessageHandler(IMessageBroadcaster webSocketMiddleware)
        {
            _webSocketMiddleware = webSocketMiddleware;
        }

        public async Task HandleIncomingMessageAsync(string body, CancellationToken cancellationToken)
        {
            try
            {
                Message? message = JsonConvert.DeserializeObject<Message>(body);

                if (message != null && message.MessageType == "report" && message.Body != null)
                {
                    Report report = message.Body.ToObject<Report>();

                    if (IsValidReport(report))
                    {
                        DataInput.StoreReport(report);
                        string jsonString = JsonConvert.SerializeObject(message.Body, Formatting.Indented, new JsonSerializerSettings
                        {
                            Converters = new List<JsonConverter> { new StringEnumConverter() },
                            NullValueHandling = NullValueHandling.Ignore
                        });
                        await _webSocketMiddleware.BroadcastMessageAsync(jsonString, message.MessageType, cancellationToken);
                    }
                    else
                    {
                        Logger.LogInfo("Invalid report received.");
                    }
                }
                else if (message != null && message.MessageType == "price" && message.Body != null)
                {
                    PriceData priceData = message.Body.ToObject<PriceData>();

                    if (IsValidPriceData(priceData))
                    {
                        DataInput.StorePrice(priceData);
                        string jsonString = JsonConvert.SerializeObject(message.Body, Formatting.Indented, new JsonSerializerSettings
                        {
                            Converters = new List<JsonConverter> { new StringEnumConverter() },
                            NullValueHandling = NullValueHandling.Ignore
                        });
                        await _webSocketMiddleware.BroadcastMessageAsync(jsonString, message.MessageType, cancellationToken);
                    }
                    else
                    {
                        Logger.LogInfo("Invalid price information received.");
                    }
                }
            }
            catch (Exception ex)
            {
                Logger.LogError(ex.ToString());
            }
        }

        private static bool IsValidReport(Report report)
        {
            return report.OrderId > 0 &&
                   !string.IsNullOrEmpty(report.ClientId) &&
                   !string.IsNullOrEmpty(report.SecurityId) &&
                   !string.IsNullOrEmpty(report.Side) &&
                   report.Price >= 0 &&
                   report.Quantity >= 0;
        }

        private static bool IsValidPriceData(PriceData priceData)
        {
            return priceData.Bid > 0 &&
                   priceData.Ask > 0 &&
                   priceData.Bid < priceData.Ask;
        }
    }
}
