using Newtonsoft.Json;

namespace MarketDataFeed
{
    public enum Event
    {
        Order,
        Trade,
        Settlement
    }

    public enum Status
    {
        Confirmed,
        Denied
    }

    public struct Report
    {
        [JsonProperty("event_type")]
        public Event EventType { get; set; }

        [JsonProperty("status")]
        public Status Status { get; set; }

        [JsonProperty("order_id")]
        public int OrderId { get; set; }

        [JsonProperty("client_id")]
        public string ClientId { get; set; }

        [JsonProperty("security_id")]
        public string SecurityId { get; set; }

        [JsonProperty("side")]
        public string Side { get; set; }

        [JsonProperty("quantity")]
        public int Quantity { get; set; }

        [JsonProperty("price")]
        public float Price { get; set; }

        [JsonProperty("reason")]
        public string? Reason { get; set; }

        public Report(
            Event eventType,
            Status status,
            int orderId,
            string clientId,
            string securityId,
            string side,
            int quantity,
            float price,
            string? reason = null
        )
        {
            EventType = eventType;
            Status = status;
            OrderId = orderId;
            ClientId = clientId;
            SecurityId = securityId;
            Side = side;
            Quantity = quantity;
            Price = price;
            Reason = reason;
        }
    }

    public struct PriceData
    {
        [JsonProperty("security_id")]
        public string SecurityId { get; set; }

        [JsonProperty("bid")]
        public float Bid { get; set; }

        [JsonProperty("ask")]
        public float Ask { get; set; }

        [JsonProperty("timestamp")]
        public string Timestamp { get; set; }

        public PriceData(
            string securityId,
            float bid,
            float ask,
            string timestamp
        )
        {
            SecurityId = securityId;
            Bid = bid;
            Ask = ask;
            Timestamp = timestamp;
        }
    }
}