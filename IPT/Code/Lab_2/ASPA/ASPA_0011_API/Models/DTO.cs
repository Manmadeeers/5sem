namespace ASPA_0011_API.Models
{
    public class CreateChannelResult
    {
        public ASP11Channel? Channel { get; set; }
        public string Status { get; set; } = null!;
    }

    public class StopAllChannelsResult
    {
        public List<ASP11Channel>? Channels { get; set; }
        public string Status { get; set; } = null!;
        public string? Reason { get; set; }
    }

    public class StopOneChannelResult
    {
        public ASP11Channel? Channel { get; set; }
        public string Status { get; set; } = null!;
    }

    public class OpenAllChannelsResult
    {
        public List<ASP11Channel>? Channels { get; set; }
        public string Status { get; set; } = null!;
    }

    public class OpenOneChannelResult
    {
        public ASP11Channel? Channel { get; set; }
        public string Status { get; set; } = null!;
    }

    public class DequeueOrPeekResult
    {
        public string? Data { get; set; }
        public string? Status { get; set; } = null!;
    }

    public class EnqueueResult
    {
        public bool Success { get; set; }
        public string Status { get; set; } = null!;
    }
}
