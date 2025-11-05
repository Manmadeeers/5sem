using ASPA_0011_API.Models;
using System.Collections.Concurrent;
using System.Threading.Channels;

namespace ASPA_0011_API.Services
{


    public interface IChannleService
    {
        List<ChannelInfo> GetAll();
        ChannelInfo? Get(Guid id);
        ChannelInfo Create(CreateChannel req);
        List<ChannelInfo> CloseAll(string? reason = null);
        ChannelInfo?CloseById(Guid id, string?reason=null);
        List<ChannelInfo> OpenAll();
        ChannelInfo? OpenById(Guid id);
        int DeleteAll();
        int DeleteClosed();
        Task<(bool success, Element? item, string? error)> EnqueueAsync(EnqueueElement req, int waitForSecDefault);
        Task<(bool success,Element?item,string?error)>DequeueAsync(DequeueElement req, int waitForSecDefault);
        Task<(bool success, Element? item, string? error)> PeekAsync(DequeueElement req, int waitForSecDefault);


    }
    public class ChannelService:IChannleService
    {


        public readonly ConcurrentDictionary<Guid, ChannelWrapper> _map = new();
        private readonly ILogger<ChannelService> _logger;

        public ChannelService(ILogger<ChannelService> logger)
        {
            _logger = logger;
        }


        public List<ChannelInfo> GetAll()
        {
            return _map.Values.Select(ToDTO).ToList();
        }
        public ChannelInfo?Get(Guid id)
        {
            if(!_map.TryGetValue(id,out var w))
            {
                return null;
            }
            return ToDTO(w);
        }

        public ChannelInfo Create(CreateChannel req)
        {
            var wrapper = new ChannelWrapper
            {
                Name = req.Name,
                State = req.State,
                Description = req.Description,
                Capacity = 100
            };

            var options = new BoundedChannelOptions(wrapper.Capacity);
            {
             
            };
            wrapper.Channel = Channel.CreateBounded<object>(options);
            wrapper.Count = 0;

            _map[wrapper.Id] = wrapper;
            _logger.LogInformation("Created channel with id={Id} name={Name}, state={State}", wrapper.Id, wrapper.Name, wrapper.State);
            return ToDTO(wrapper);
        }

        public List<ChannelInfo>CloseAll(string?reason = null)
        {
            var outList = new List<ChannelInfo>();
            foreach(var kv in _map)
            {
                kv.Value.State = ChannelState.CLOSED;
                outList.Add(ToDTO(kv.Value));
                _logger.LogInformation("Closed channel with id={Id}. Reason={reason}", kv.Key, reason);
            }
            return outList;
        }

        public ChannelInfo?CloseById(Guid id,string? reason=null)
        {
            if(!_map.TryGetValue(id,out var w))
            {
                return null;
            }
            w.State = ChannelState.CLOSED;
            _logger.LogInformation("Channel with id={Id} closed", id);
            return ToDTO(w);
        }

        public List<ChannelInfo> OpenAll()
        {
            var outList = new List<ChannelInfo>();
            foreach(var kv in _map)
            {
                kv.Value.State = ChannelState.ACTIVE;
                outList.Add(ToDTO(kv.Value));
                _logger.LogInformation("Channel with id={Id} opened", kv.Value.Id);
            }
            return outList;
        }

        public ChannelInfo?OpenById(Guid id)
        {
            if(!_map.TryGetValue(id,out var w))
            {
                return null;
            }
            w.State = ChannelState.ACTIVE;
            _logger.LogInformation("Opened channel with id {id}", id);
            return ToDTO(w);
        }

        public int DeleteAll()
        {
            var keys = _map.Keys.ToList();
            foreach(var k in keys)
            {
                if(_map.TryRemove(k,out var w))
                {
                    w.Channel.Writer.TryComplete();
                    _logger.LogInformation("Deleted channel with id {id}", k);  
                }
            }
            return keys.Count;
        }

        public int DeleteClosed()
        {
            var toRemove = _map.Where(x => x.Value.State == ChannelState.CLOSED).Select(x => x.Key).ToList();
            foreach(var k in toRemove)
            {
                if(_map.TryRemove(k,out var w))
                {
                    w.Channel.Writer.TryComplete();
                    _logger.LogInformation("Deleted closed channel with id {id}", k);
;               }
            }

            return toRemove.Count;
        }

        public async Task<(bool success, Element?item, string?error)>EnqueueAsync(EnqueueElement req, int waitSecondsDefault)
        {
            if(!_map.TryGetValue(req.Id,out var w))
            {
                _logger.LogError("Enqueue: Channel not found ({id})", req.Id);
                return (false, null, "Channel not found");
            }

            if (w.State != ChannelState.ACTIVE)
            {
                _logger.LogWarning("Enqueue: channel not active ({id})", req.Id);
                return (false, null, "Channel not active");
            }
            var writer = w.Channel.Writer;
            using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(waitSecondsDefault));
            try
            {
                if(!await writer.WaitToWriteAsync(cts.Token)){
                    _logger.LogWarning("Enqueue timeout waiting to write ({id})", req.Id);
                    return (false, null, "Enqueue timeout");
                }

                var payload = req.Data;
                var success = writer.TryWrite(payload);
                if (!success)
                {
                    _logger.LogWarning("Enqueue TryWrite failed {id}", req.Id);
                    return (false, null, "Enqueue failed");
                }
                Interlocked.Increment(ref w.Count);
                _logger.LogInformation("Enqueued to {Id}", req.Id);
                return (true, null, null);
            }
            catch(OperationCanceledException ex)
            {
                _logger.LogWarning("Enqueue operation canceled (timeout) {id}", req.Id);
                return (false, null, "Enqueue timeout");
            }
        }

        public async Task<(bool success, Element?item,string?error)>DequeueAsync(DequeueElement req,int waitSecondsDefault)
        {
            if(!_map.TryGetValue(req.Id,out var w))
            {
                _logger.LogError("Dequeue: Channel not found {Id}", req.Id);
                return (false, null, "Channel not found");
            }
            var reader = w.Channel.Reader;
            if(reader.TryRead(out var obj))
            {
                Interlocked.Decrement(ref w.Count);
                var el = new Element { Id = req.Id, Data = obj?.ToString() ?? string.Empty };
                _logger.LogInformation("Dequeued from {Id}", req.Id);
                return (true, el, null);
            }
            using var cts =new CancellationTokenSource(TimeSpan.FromSeconds(1));
            try
            {
                if(await reader.WaitToReadAsync(cts.Token))
                {
                    if(reader.TryRead(out var it))
                    {
                        Interlocked.Decrement(ref w.Count);
                        var el = new Element { Id = req.Id, Data = it?.ToString() ?? string.Empty };
                        _logger.LogInformation("Dequeued after wait from {Id}", req.Id);
                        return (true, el, null);
                    }
                }

            }
            catch(OperationCanceledException ex)
            {
                _logger.LogWarning("Operation canceled {Id}", req.Id);
                return (false, null, "Operation canceled");
            }
        }

        public async Task<(bool success,Element?item,string?error)>PeekAsync(DequeueElement req, int waitSecondsDefault)
        {
            if(!_map.TryGetValue(req.Id,out var w))
            {
                _logger.LogError("Peek:channel not found {Id}", req.Id);
                return (false, null, "Channel not found");
            }
            var reader = w.Channel.Reader;
            if(reader.TryRead(out var obj))
            {
                var writer = w.Channel.Writer;
                var wroteBack = writer.TryWrite(obj);
                if (!wroteBack)
                {
                    _logger.LogWarning("Peek:couldn't write back item for {id}", req.Id);

                }
                var el = new Element { Id=req.Id,Data = obj?.ToString() ?? string.Empty };
                _logger.LogInformation("Peek item in {Id}", req.Id);
                return (true, el, null);
            }

            using var cts = new CancellationTokenSource(TimeSpan.FromSeconds(1));

            try
            {
                if(await reader.WaitToReadAsync(cts.Token))
                {
                    if(reader.TryRead(out var obj2))
                    {
                        var writer = w.Channel.Writer;
                        var wroteBacck = writer.TryWrite(obj2);
                        if (!wroteBacck)
                        {
                            _logger.LogWarning("Peek: couldn't write back after wait on {Id}", req.Id);

                        }
                        var el = new Element { Id = req.Id, Data = obj2?.ToString() ?? string.Empty };
                        _logger.LogInformation("Peek after wait on {Id}", req.Id);
                        return (true, el, null);
                    }
                }
                return (false, null, "Queue empty");    

            }
            catch(OperationCanceledException ex)
            {
                _logger.LogWarning("Peek:operation canceled {Id}", req.Id);
                return (false, null, "Operation canceled");
            }
        }



        private static ChannelInfo ToDTO(ChannelWrapper wrapper)
        {
            return new ChannelInfo
            {
                Id = wrapper.Id,
                Name = wrapper.Name,
                State = wrapper.State,
                Description = wrapper.Description
            };

        }

    }
}
