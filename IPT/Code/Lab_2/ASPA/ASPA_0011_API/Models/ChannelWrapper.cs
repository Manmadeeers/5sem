using System.Threading.Channels;

namespace ASPA_0011_API.Models
{
    public class ChannelWrapper
    {
        public Guid Id { get; } = Guid.NewGuid();
        public string Name { get; set; } = default!;
        public Channel<object> Channel { get; set; } = default!;
        public ChannelState State { get; set; }
        public string Description { get; set; } = string.Empty;
        public DateTime CreatedUTC { get; set; } = DateTime.UtcNow;
        public int Capacity { get; set; } = 100;
        public int Count;
    }
}
