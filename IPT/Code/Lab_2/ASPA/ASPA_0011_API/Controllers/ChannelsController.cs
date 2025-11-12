using ASPA_0011_API.Models;
using ASPA_0011_API.Services;
using Microsoft.AspNetCore.Mvc;

namespace ASPA_0011_API.Controllers
{
    [ApiController]
    [Route("api/channels")]
    public class ChannelsController : ControllerBase
    {

        private readonly ChannelService _channelService;
        private ILogger<ChannelService> _logger;
        private int _eventCounter = 0;

        public ChannelsController(ChannelService channelService, ILogger<ChannelService> logger)
        {
            _channelService = channelService;
            _logger = logger;
        }

        [HttpGet("get-all")]
        [ProducesResponseType(200)]
        [ProducesResponseType(204)]
        public ActionResult<List<ASP11Channel>> GetAll()
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - GetAll", ++_eventCounter, DateTime.Now);

            var channels = _channelService.GetAllChannels();
            if (channels == null || channels.Count == 0)
            {
                return NoContent();//204    
            }

            return Ok(channels);//200
        }

        [HttpGet("{id}")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public ActionResult<ASP11Channel> GetById(Guid id)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - GetById", ++_eventCounter, DateTime.Now);

            var channel = _channelService.GetChannelById(id);
            if (channel == null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - GetById, channel was null", ++_eventCounter, DateTime.Now);
                return NotFound();//404
            }

            return Ok(channel);//200
        }

        [HttpPost("create")]
        [ProducesResponseType(201)]
        [ProducesResponseType(204)]
        [ProducesResponseType(400)]
        public ActionResult<ASP11Channel> CreateChannel([FromBody] Models.CreateChannel model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - CreateChannel",
            ++_eventCounter, DateTime.Now);
            var result = _channelService.CreateChannel(model);

            if (result.Channel == null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - CreateChannel, channel == null",
                ++_eventCounter, DateTime.Now);

                return BadRequest(result.Status);
            }

            //if (model.State == ChannelState.ACTIVE)
            //{
            //    return NoContent();
            //}

            return CreatedAtAction(nameof(GetById), new { id = result.Channel.Id }, result.Channel);

        }

        [HttpPost("dequeue-or-peek")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]

        public ActionResult<DequeueOrPeekResult> DequeueOrPeek([FromBody] Models.QueueCommand model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - DequeueOrPeek",
            ++_eventCounter, DateTime.Now);

            var result = _channelService.DequeueOrPeek(model);
            if (result.Data == null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - DequeueOrPeek, data == null",
                ++_eventCounter, DateTime.Now);
                return NotFound(result.Status);
            }

            return Ok(result);
        }

        [HttpPost("enqueue")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        public IActionResult Enqueue([FromBody] Models.EnqueueModel model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - Enqueue",
            ++_eventCounter, DateTime.Now);
            var result = _channelService.Enqueue(model);
            if (result.IsCompletedSuccessfully)
            {
                return Ok(result.Status);
            }

            _logger.LogError("[{EventId}] {Timestamp} - Enqueue, not completed successfully",
            ++_eventCounter, DateTime.Now);
            return NotFound(result.Status);
        }

        [HttpPut("stop-all")]
        [ProducesResponseType(200)]
        [ProducesResponseType(400)]
        public ActionResult<StopAllChannelsResult> StopAllChannels([FromBody] Models.StopAllChannels model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - StopAllChannels",
            ++_eventCounter, DateTime.Now);

            var result = _channelService.StopAllChannels(model);
            if (result.Channels == null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - StopAllChannels, no channels to stop",
                ++_eventCounter, DateTime.Now);

                return BadRequest(result.Status);
            }

            return Ok(result);
        }

        [HttpPut("stop-one")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        [ProducesResponseType(400)]
        public ActionResult<StopOneChannelResult> StopOneChannel([FromBody]Models.StopChannelById model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - StopOneChannel",
            ++_eventCounter, DateTime.Now);

            var result = _channelService.StopOneChannel(model);
            if (result.Channel == null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - StopOneChannel, channel == null",
                ++_eventCounter, DateTime.Now);

                if(result.Status.Contains("Wrong command"))
                {
                    return BadRequest(result.Status);
                }
                return NotFound(result.Status);
            }

            return Ok(result);
        }

        [HttpPut("open-all")]
        [ProducesResponseType(200)]
        [ProducesResponseType(400)]
        public ActionResult<OpenAllChannelsResult> OpenAll([FromBody]Models.OpenAllChannels model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - OpenAll",
            ++_eventCounter, DateTime.Now);

            var result = _channelService.OpenAllChannels(model);
            if (result.Channels == null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - ResumeAll, channels == null",
                ++_eventCounter, DateTime.Now);
                return BadRequest(result.Status);
            }

            return Ok(result);
        }

        [HttpPut("open-one")]
        [ProducesResponseType(200)]
        [ProducesResponseType(404)]
        [ProducesResponseType(400)]
        public ActionResult<OpenOneChannelResult> OpenOne([FromBody]Models.OpenChannelById model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - ResumeOne",
            ++_eventCounter, DateTime.Now);

            var result = _channelService.OpenOneChannel(model);
            if (result.Channel == null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - ResumeOne, channel == null",
                ++_eventCounter, DateTime.Now);

                if (result.Status.Contains("Command not found"))
                {
                    return BadRequest(result.Status);
                }
                return NotFound(result.Status);
            }

            return Ok(result);
        }

        [HttpDelete("delete-all")]
        [ProducesResponseType(204)]
        [ProducesResponseType(400)]
        public IActionResult DeleteAll([FromBody]Models.DeleteAllChannels model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - DeleteAll",
            ++_eventCounter, DateTime.Now);

            var result = _channelService.DeleteAllChannels(model);
            if (result != null)
            {
                _logger.LogError("[{EventId}] {Timestamp} - Delete all, wrong command",
                ++_eventCounter, DateTime.Now);

                return BadRequest(result);
            }

            return NoContent();
        }

        [HttpDelete("delete-closed")]
        [ProducesResponseType(typeof(List<ASP11Channel>), 200)]
        public ActionResult<List<ASP11Channel>> DeleteClosed([FromBody]Models.DeleteClosedChannels model)
        {
            _logger.LogTrace("[{EventId}] {Timestamp} - DeleteClosed",
            ++_eventCounter, DateTime.Now);

            var channels = _channelService.DeleteCloedChannels(model);
            return Ok(channels);
        }
    }
}
