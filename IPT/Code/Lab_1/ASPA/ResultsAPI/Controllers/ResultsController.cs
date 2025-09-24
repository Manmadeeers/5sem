using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using ResultsAPI.Models;

namespace ResultsAPI.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class ResultsController : ControllerBase
    {
        public static readonly List<Result> Resulst = new List<Result>();


        [HttpGet]
        public ActionResult<IEnumerable<Result>> GetResults()
        {
            return Ok(Resulst);
        }

        [HttpGet("{id}")]
        public ActionResult<Result> GetResult(int id)
        {
            var result = Resulst.FirstOrDefault(r => r.Key == id);
            if(result is null)
            {
                return NotFound();
            }
            return Ok(result);
        }
    }
}
