using Microsoft.AspNetCore.Mvc;
using ASPA_0011_API.Models;
using ASPA_0011_API.Services;

namespace ASPA_0011_API.Controllers
{
    [ApiController]
    [Route("[controller]")]
    public class ChannelsController : ControllerBase
    {
        public readonly ChanelService _channelService;
        public ChannelsController(Services.ChanelService channelService) { 
            this._channelService = channelService;
        }



    }
}
