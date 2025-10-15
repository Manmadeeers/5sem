using ASPA_0010_1.Models;
using BSTU.Results.Authenticate.Services;
using BSTU.Results.Collection.Models;
using BSTU.Results.Collection.Services;
using Microsoft.AspNet.Identity.EntityFramework;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;

namespace ASPA_0010_1.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ResultsController : ControllerBase
    {
        private readonly ResultsService _resultsService;
        private readonly AuthenticationService _authenticationService;
        public ResultsController(ResultsService resultsService, AuthenticationService authenticationService)
        {
            _resultsService = resultsService;
            _authenticationService = authenticationService;
        }
        [HttpPost("SignIn")]
        public async Task<ActionResult> SignIn([FromBody] UserProfile profile)
        {
            if (string.IsNullOrEmpty(profile.Username) || string.IsNullOrEmpty(profile.Password))
            {
                return BadRequest();
            }
            var res = await _authenticationService.SignInAsync(profile.Username, profile.Password);
            if (!res.Succeeded)
            {
                return NotFound();
            }
            return Ok(res);
        }
        [HttpGet("SignOut")]
        [Authorize]
        public async Task<ActionResult> SignOut()
        {
            await _authenticationService.SignOut();
            return Ok("Signed Out");
        }

        [HttpGet]
        [Authorize(Roles = "READER")]
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
        [Authorize(Roles = "READER")]
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
        [Authorize(Roles ="WRITER")]
        public ActionResult Post([FromBody]Result result)
        {
            if(_resultsService.AddSync(result))
            {
                return CreatedAtAction(nameof(Get), result);
            }
            return BadRequest();
          
        }

        [HttpPut("{key:int}")]
        [Authorize(Roles = "WRITER")]
        public ActionResult Put([FromQuery]int key, [FromBody]string value)
        {
            if( _resultsService.UpdateSync(key, value))
            {
                return Ok();
            }
            return BadRequest();
        }

        [HttpDelete("{key:int}")]
        [Authorize(Roles = "WRITER")]
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
