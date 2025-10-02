using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using ResultsAPI.Models;
using ResultsAPI.Service;

namespace ResultsAPI.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ResultsController : ControllerBase
    {
        public readonly ResultsService _resultsService;
        public ResultsController(ResultsService resultsService)
        {
            _resultsService = resultsService;
        }

        [HttpGet]
        public async Task<IActionResult> GetAllResults()
        {
            var results = await _resultsService.GetAllResultsAsync();

           return Ok(results);
        }

        [HttpGet("{k:int}")]
        public async Task<IActionResult>GetResultByKey(int key)
        {
            var result = await _resultsService.GetByKeyAsync(key);

            if (result == null)
            {
                return NotFound();
            }
            else
            {
                return Ok(result);
            }
        }

        [HttpPost]
        public async Task<IActionResult>AddResult([FromBody]string value)
        {
            if (string.IsNullOrWhiteSpace(value))
            {
                return BadRequest();
            }
            var addResult = await _resultsService.AddSync(value);
            return CreatedAtAction(nameof(GetResultByKey), new { Key = addResult.Key }, addResult);
        }

        [HttpPut("{key:int}")]
        public async Task<IActionResult>UpdateResult(int key, [FromBody]string value)
        {
            if (string.IsNullOrWhiteSpace(value))
            {
                return BadRequest();
            }

            var updatedResult = await _resultsService.UpdateAsync(key, value);
            if(updatedResult == null)
            {
                return NotFound();
            }
            else
            {
                return Ok(updatedResult);
            }
        }

        [HttpDelete("{id:int}")]
        public async Task<IActionResult>DeleteResult(int id)
        {
            var deletionResult = await _resultsService.DeleteAsync(id);
            if (!deletionResult)
            {
                return NotFound();
            }
            else
            {
                return Ok();
            }
        }
    }
}
