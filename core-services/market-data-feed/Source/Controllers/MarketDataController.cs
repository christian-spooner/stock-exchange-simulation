using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;

namespace MarketDataFeed.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class MarketDataController : ControllerBase
    {
        [HttpGet("status/{securityId}/{orderId}")]
        public IActionResult GetStatus(string securityId, string orderId)
        {
            string status = DataAccess.GetLatestStatus(securityId, orderId);
            return Ok(status);
        }

        [HttpGet("report/{securityId}/{orderId}")]
        public IActionResult GetReport(string securityId, string orderId)
        {
            string report = DataAccess.GetLatestReport(securityId, orderId);
            return Ok(report);
        }

        [HttpGet("price/{securityId}")]
        public IActionResult GetPrice(string securityId)
        {
            string price = DataAccess.GetLatestPrice(securityId);
            return Ok(price);
        }
    }
}