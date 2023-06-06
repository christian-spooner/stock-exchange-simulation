using Microsoft.AspNetCore.Mvc;

namespace MarketDataFeed.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class TestController : ControllerBase
    {
        [HttpGet("healthCheck")]
        public IActionResult GetHealthCheck()
        {
            return Ok("Hello from built-in REST API!\n");
        }
    }
}