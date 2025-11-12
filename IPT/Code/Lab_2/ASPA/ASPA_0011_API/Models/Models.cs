
namespace ASPA_0011_API.Models
{
    public enum ChannelState { ACTIVE, CLOSED}

    public class ASP11Channel
    {
        public Guid Id { get; set; }
        public string Name { get; set; }
        public string Description { get; set; }
        public ChannelState State { get; set; }
    }

    public class CreateChannel
    {
        public string Command { get; set; }
        public string Name { get; set; }
        public ChannelState State { get; set; }
        public string Description { get; set; }
    }

    public class StopAllChannels
    {
        public string Command { get; set; }
        public string Reason { get; set; }
    }

    public class StopChannelById
    {
        public Guid Id { get; set; }
        public string Command { get; set; }
        public string Reason { get; set; }

    }


    public class OpenAllChannels
    {
        public string Command { get; set; }
    }


    public class OpenChannelById
    {
        public Guid Id { get; set; }
        public string Command { get; set; }
        public ChannelState State { get; set; }
    }


    public class DeleteAllChannels
    {
        public string Command { get; set; } 
    }


    public class DeleteClosedChannels
    {
        public string Command { get; set; }
        public ChannelState State { get; set; }
    }

    public class QueueCommand
    {
        public string Command { get; set; }
        public Guid Id { get; set; }
    }

    public class EnqueueModel
    {
        public string Command { get; set; }
        public Guid Id { get; set; }
        public string Data { get; set; }
    }

    public class QueueErrorModel
    {
        public Guid Id { get; set; }
        public string Message { get; set; }
    }

}
