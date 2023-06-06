using Microsoft.AspNetCore.Mvc;
using System.Threading.Tasks;

namespace MarketDataFeed.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class AccountDataController : ControllerBase
    {
        [HttpGet("balance/{clientId}")]
        public IActionResult GetBalance(string clientId)
        {
            string balance = DataAccess.GetBalance(clientId);
            return Ok(balance);
        }

        [HttpGet("holdings/{securityId}/{clientId}")]
        public IActionResult GetHoldings(string securityId, string clientId)
        {
            string holdings = DataAccess.GetHoldings(securityId, clientId);
            return Ok(holdings);
        }
    }
}