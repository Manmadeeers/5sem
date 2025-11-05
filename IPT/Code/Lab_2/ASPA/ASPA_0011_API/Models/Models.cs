
namespace ASPA_0011_API.Models
{
    public enum ChannelState { ACTIVE, CLOSED}
    public enum QueueOperations { ENQUEUE,DEQUEUE,PEEK}
    public class ChannelInfo
    {
        public Guid Id { get; set; }
        public string Name { get; set; }= default!;
        public ChannelState State { get; set; }
        public string Description { get; set; } = default!;
    }

    public class CreateChannel
    {
        public string Command { get; set; } = "new";
        public string Name { get; set; } = default!;
        public ChannelState State { get; set; }
        public string Description { get; set; } = string.Empty;
    }

    public class CloseAllChannels
    {
        public string Command { get; set; } = "close";
        public string Reason { get; set; }=string.Empty;
    }

    public class CloseChannelById
    {
        public Guid Id { get; set; }
        public string Command { get; set; } = "close";
        public string Reason { get; set; } = string.Empty;

    }

    public class OpenAllChannels
    {
        public string Command { get; set; } = "open";
    }

    public class OpenChannelById
    {
        public Guid Id { get; set; }
        public string Command { get; set; } = "open";
        public ChannelState State { get; set; } = ChannelState.ACTIVE;
    }


    public class DeleteAllChannels
    {
        public string Command { get; set; } = "delete";
    }

    public class DeleteClosedChannels
    {
        public string Command { get; set; } = "delete";
        public ChannelState State { get; set; } = ChannelState.CLOSED;
    }

    public class DequeueElement
    {
        public QueueOperations Command { get; set; } //DEQUEUE, PEEK
        public Guid Id { get; set; }

    }

    public class EnqueueElement
    {
        public QueueOperations Command { get; set; } = QueueOperations.ENQUEUE;
        public Guid Id { get; set; }
        public string Data { get; set; } = default!; //JSON string
    }


    public class Element
    {
        public Guid Id { get; set; }
        public string Data { get; set; }= default!; //JSON string
    }

    public class ErrorMessage
    {
        public Guid Id { get; set; }
        public string Error { get; set; } = default!;
    }

}
