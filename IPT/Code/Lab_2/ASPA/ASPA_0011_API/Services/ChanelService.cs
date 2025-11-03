using ASPA_0011_API.Models;
using Microsoft.AspNetCore.Mvc;

namespace ASPA_0011_API.Services
{
    public class ChanelService
    {
        public readonly List<QueueElement> _elements = new List<QueueElement>();

        public List<QueueElement> ? GetElements()
        {
            return _elements;
        }

    }
}
