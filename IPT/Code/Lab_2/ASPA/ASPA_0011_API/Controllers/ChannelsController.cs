using ASPA_0011_API.Models;
using ASPA_0011_API.Services;
using Microsoft.AspNetCore.Mvc;

namespace ASPA_0011_API.Controllers
{
    [ApiController]
    [Route("api/channels")]
    public class ChannelsController : ControllerBase
    {

        private readonly IChannleService _srvc;
        private readonly ILogger<ChannelsController> _logger;

        public ChannelsController(IChannleService srvc, ILogger<ChannelsController> logger)
        {
            _srvc = srvc;
            _logger = logger;
        }

        //GET /api/channels
        [HttpGet]
        public ActionResult<List<ChannelInfo>> Get()
        {
            var all = _srvc.GetAll();
            if (all == null)
            {
                return NotFound();
            }
            if (all.Count == 0)
            {
                return NoContent();
            }
            return Ok(all);
        }

        //GET /api/channels/{id}
        [HttpGet("{id:guid}")]
        public ActionResult<ChannelInfo>Get(Guid id)
        {
            var channel = _srvc.Get(id);
            if (channel == null)
            {
                return NotFound();
            }
            return Ok(channel);
        }

        //POST /api/channels - CREATE CHANNEL
        [HttpPost]
        public ActionResult<ChannelInfo> Post([FromBody]CreateChannel req)
        {
            var created = _srvc.Create(req);
            if (created.State == ChannelState.ACTIVE)
            {
                return CreatedAtAction(nameof(Get), new { id = created.Id }, created);
            }
            return NoContent();
        }

        //PUT /api/channels - OPEN/CLOSE 
        [HttpPut]
        public ActionResult Put([FromBody]object command)
        {

        }
    }
}
