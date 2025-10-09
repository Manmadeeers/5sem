using Microsoft.AspNetCore.Mvc;
using ResultAPI.Models;
using ResultAPI.Services;


namespace ResultAPI.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ResultsController:ControllerBase
    {
        private readonly ResultsService _resultsService;
        public ResultsController(ResultsService resultsService)
        {
            _resultsService = resultsService;
        }

        [HttpGet]
        public async Task<ActionResult<List<Result>>> Get()
        {
            var results = _resultsService.GetAllSync();
            if (results == null)
            {
                return NotFound();
            }
            return Ok(results);
        }
        [HttpGet("{key:int}")]
        public async Task<ActionResult<Result>> Get(int key)
        {
            var result = _resultsService.GetResultSync(key);
            if (result != null)
            {
                return result;
            }
            return NotFound();
        }

        [HttpPost]
        public async Task<ActionResult<Result>> Post([FromBody]Result result)
        {
            if(await _resultsService.AddSync(result))
            {
                return CreatedAtAction(nameof(Get), result);
            }
            return BadRequest();
          
        }

        [HttpPut("{key:int}")]
        public async Task<ActionResult<Result>>Put(int key, [FromBody]string value)
        {
            if(await _resultsService.UpdateSync(key, value))
            {
                return Ok();
            }
            return BadRequest();
        }

        [HttpDelete("{key:int}")]
        public async Task<ActionResult<Result>>Delete(int key)
        {
            if(await _resultsService.DeleteSync(key))
            {
                return Ok();
            }
            return BadRequest();
        }
    }
}
