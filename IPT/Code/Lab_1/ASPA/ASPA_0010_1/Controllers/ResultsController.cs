using Microsoft.AspNetCore.Mvc;
using BSTU.Results.Collection.Models;
using BSTU.Results.Collection.Services;

namespace ASPA_0010_1.Controllers
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
            var results = await _resultsService.GetAllAsync();
            if (results == null)
            {
                return NotFound();
            }
            return Ok(results);
        }
        [HttpGet("{key:int}")]
        public async Task<ActionResult<Result>> Get(int key)
        {
            var result = await _resultsService.GetResultAsync(key);
            if (result != null)
            {
                return result;
            }
            return NotFound();
        }

        [HttpPost]
        public async Task<ActionResult<Result>> Post([FromBody]Result result)
        {
            if(await _resultsService.AddAsync(result))
            {
                return CreatedAtAction(nameof(Get), result);
            }
            return BadRequest();
          
        }

        [HttpPut("{key:int}")]
        public async Task<ActionResult<Result>>Put(int key, [FromBody]string value)
        {
            if(await _resultsService.UpdateAsync(key, value))
            {
                return Ok();
            }
            return BadRequest();
        }

        [HttpDelete("{key:int}")]
        public async Task<ActionResult<Result>>Delete(int key)
        {
            if(await _resultsService.DeleteAsync(key))
            {
                return Ok();
            }
            return BadRequest();
        }
    }
}
