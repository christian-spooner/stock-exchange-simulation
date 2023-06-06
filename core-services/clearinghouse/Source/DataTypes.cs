using Newtonsoft.Json;

namespace Clearinghouse
{
    public struct Trade
    {
        [JsonProperty("order_id")]
        public int OrderId { get; set; }

        [JsonProperty("client_id")]
        public string ClientId { get; set; }

        [JsonProperty("security_id")]
        public string SecurityId { get; set; }

        [JsonProperty("side")]
        public string Side { get; set; }

        [JsonProperty("price")]
        public decimal Price { get; set; }

        [JsonProperty("quantity")]
        public int Quantity { get; set; }

    }

    public struct Report
    {
        public string event_type;
        public string status;
        public int order_id;
        public string client_id;
        public string security_id;
        public string side;
        public int quantity;
        public decimal price;
        public string? reason;
    }
}