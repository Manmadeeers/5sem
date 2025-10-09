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
        public ActionResult<List<Result>> Get()
        {
            var results =  _resultsService.GetAllSync();
            if (results == null)
            {
                return NotFound();
            }
            return Ok(results);
        }
        [HttpGet("{key:int}")]
        public ActionResult<Result> Get(int key)
        {
            var result = _resultsService.GetResultSync(key);
            if (result != null)
            {
                return result;
            }
            return NotFound();
        }

        [HttpPost]
        public ActionResult Post([FromBody]Result result)
        {
            if(_resultsService.AddSync(result))
            {
                return CreatedAtAction(nameof(Get), result);
            }
            return BadRequest();
          
        }

        [HttpPut("{key:int}")]
        public ActionResult Put([FromQuery]int key, [FromBody]string value)
        {
            if( _resultsService.UpdateSync(key, value))
            {
                return Ok();
            }
            return BadRequest();
        }

        [HttpDelete("{key:int}")]
        public ActionResult Delete(int key)
        {
            if( _resultsService.DeleteSync(key))
            {
                return Ok();
            }
            return BadRequest();
        }
    }
}
