using System.Threading.Channels;
using ASPA_0011_API.Models;


namespace ASPA_0011_API.Services
{

    public class ChannelService
    {
        private ILogger<ChannelService> _logger;
        private int _waitEnqueue;
        private int _eventCounter = 0;
        private List<ASP11Channel> _asp11Channels = new List<ASP11Channel>();
        private List<Channel<string>> _channels = new List<Channel<string>>();

        public ChannelService(IConfiguration c, ILogger<ChannelService> logger)
        {
            _logger = logger??throw new ArgumentNullException(nameof(logger));
            _waitEnqueue = c.GetValue<int>("WaitEnqueue", 30);
            _logger.LogTrace("[{EventId}] {TimeStamp} - ChannelService started", ++_eventCounter, DateTime.Now);
        }

        public List<ASP11Channel> GetAllChannels()
        {
            return this._asp11Channels;
        }

        public ASP11Channel? GetChannelById(Guid id)
        {
            return this._asp11Channels.FirstOrDefault(c => c.Id == id);
        }

        public CreateChannelResult CreateChannel(CreateChannel newChannel)
        {
            _logger.LogDebug("[{EventId}] {TimeStamp} - CreateChannel {Command}", ++_eventCounter, DateTime.Now, newChannel.Command);

            if (newChannel.Command == "new")
            {
                var id = Guid.NewGuid();
                var channel = Channel.CreateUnbounded<string>();
                _channels.Add(channel);

                var aspChannel = new ASP11Channel
                {
                    Id = id,
                    Name = newChannel.Name,
                    Description = newChannel.Description,
                    State = newChannel.State,
                };
                _asp11Channels.Add(aspChannel);

                _logger.LogInformation("[{EventId}] {Timestamp} - Channel created: {ChannelId}", ++_eventCounter, DateTime.Now, id);

                return new CreateChannelResult
                {
                    Channel = aspChannel,
                    Status = "success"
                };
            }
            else
            {
                return new CreateChannelResult
                {
                    Channel = null,
                    Status = "Wrong command. Try again"
                };
            }
        }

        public StopAllChannelsResult StopAllChannels(Models.StopAllChannels model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} - StopAllChannels {Command}", ++_eventCounter, DateTime.Now, model.Command);

            if (model.Command == "close")
            {
                for(int i = 0; i < _channels.Count; i++)
                {
                    if (_asp11Channels[i].State != ChannelState.CLOSED)
                    {
                        _channels[i].Writer.Complete();
                        _asp11Channels[i].State = ChannelState.CLOSED;
                    }

                }

                return new StopAllChannelsResult
                {
                    Channels = _asp11Channels,
                    Status = "success",
                    Reason = model.Reason
                };
            }

            return new StopAllChannelsResult
            {
                Channels = null,
                Status = "Wrong command. Try again",
                Reason = null
            };
        }

        public StopOneChannelResult StopOneChannel(Models.StopChannelById model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} - StopOneChannels {Command}", ++_eventCounter, DateTime.Now, model.Command);

            if (model.Command == "close")
            {
                int index = _asp11Channels.FindIndex(c => c.Id == model.Id);
                if (index == -1)
                {
                    return new StopOneChannelResult
                    {
                        Channel = null,
                        Status = "Channel not found"
                    };
                }
                if (_asp11Channels[index].State == ChannelState.ACTIVE)
                {
                    _channels[index].Writer.Complete();
                    _asp11Channels[index].State = ChannelState.CLOSED;
                }

                return new StopOneChannelResult
                {
                    Channel = _asp11Channels[index],
                    Status = "success"
                };
            }

            return new StopOneChannelResult
            {
                Channel = null,
                Status = "Wrong command. Try again"
            };
        }

        public OpenAllChannelsResult OpenAllChannels(Models.OpenAllChannels model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} -OpensAllChannels {Command}", ++_eventCounter, DateTime.Now, model.Command);


            if (model.Command == "open")
            {
                for(int i = 0; i < _channels.Count; i++)
                {
                    if (_asp11Channels[i].State == ChannelState.CLOSED)
                    {
                        var newChannel = Channel.CreateUnbounded<string>();
                    }
                }

                return new OpenAllChannelsResult
                {
                    Channels = _asp11Channels,
                    Status = "success"
                };
            }
            return new OpenAllChannelsResult
            {
                Channels = null,
                Status = "Wrong command.Try again"
            };

        }


        public OpenOneChannelResult OpenOneChannel(Models.OpenChannelById model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} - OpenOneChannel {Command}", ++_eventCounter, DateTime.Now, model.Command);

            if (model.Command == "open")
            {
                int index = _asp11Channels.FindIndex(c=>c.Id == model.Id);
                if(index == -1)
                {
                    return new OpenOneChannelResult
                    {
                        Channel = null,
                        Status = "No channel found"
                    };
                }
                if (_asp11Channels[index].State == ChannelState.CLOSED)
                {
                    _channels[index] = Channel.CreateUnbounded<string>();
                    _asp11Channels[index].State = model.State;
                }

                return new OpenOneChannelResult
                {
                    Channel = _asp11Channels[index],
                    Status = "success"
                };
            }

            return new OpenOneChannelResult
            {
                Channel = null,
                Status = "Wrong comman. Try again"
            };
        }

        public string?DeleteAllChannels(Models.DeleteAllChannels model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} - DeleteAllChannels {Command}", ++_eventCounter, DateTime.Now, model.Command);

            if (model.Command == "delete")
            {
                foreach(var channel in _channels)
                {
                    try
                    {
                        channel.Writer.Complete();
                    }
                    catch(Exception ex)
                    {
                        Console.Write(ex.ToString());
                    }

                    _channels.Clear();
                    _asp11Channels.Clear();
                    return null;
                }
            }
            return "Wrong command.Try again";
        }

        public List<ASP11Channel> DeleteCloedChannels(Models.DeleteClosedChannels model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} - DeleteClosedChannels {Command}", ++_eventCounter, DateTime.Now, model.Command);

            for(int i=_channels.Count-1; i>=0; i--)
            {
                if (_asp11Channels[i].State == ChannelState.CLOSED)
                {
                    try
                    {
                        _channels[i].Writer.Complete();
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.ToString());
                    }

                    _channels.RemoveAt(i);
                    _asp11Channels.RemoveAt(i);
                }
            }
            return _asp11Channels;
        }

        public DequeueOrPeekResult DequeueOrPeek(Models.QueueCommand model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} - DequeueOrPeek {Command}", ++_eventCounter, DateTime.Now, model.Command);

            int index = _asp11Channels.FindIndex(c => c.Id == model.Id);
            if (index == -1)
            {
                return new DequeueOrPeekResult
                {
                    Data = null,
                    Status = "No channel found"
                };
            }
            var channel = _channels[index];

            if (model.Command == "dequeue")
            {
                if(channel.Reader.TryRead(out var value))
                {
                    return new DequeueOrPeekResult
                    {
                        Data = value,
                        Status = "TryRead success"
                    };
                }
                else
                {
                    return new DequeueOrPeekResult
                    {
                        Data = null,
                        Status = "TryRead failed"
                    };
                }
            }
            else if (model.Command == "peek")
            {
                if(channel.Reader.TryPeek(out var value))
                {
                    return new DequeueOrPeekResult
                    {
                        Data = value,
                        Status = "TryPeek success"
                    };
                }
                else
                {
                    return new DequeueOrPeekResult
                    {
                        Data = value,
                        Status = "TryPeek failed"
                    };
                }
            }
            else
            {
                return new DequeueOrPeekResult
                {
                    Data = null,
                    Status = "Wrong command. Try again"
                };
            }
        }

        public async Task<EnqueueResult>Enqueue(Models.EnqueueModel model)
        {
            _logger.LogDebug("[{EventId}] {Timestamp} - Enqueue {Command}", ++_eventCounter, DateTime.Now, model.Command);

            if (model.Command != "enqueue")
            {
                return new EnqueueResult
                {
                    Success = false,
                    Status = "Wrong command. Try again"
                };
            }

            int index = _asp11Channels.FindIndex(c => c.Id == model.Id);
            if (index == -1)
            {
                return new EnqueueResult
                {
                    Success = false,
                    Status = "No channels found"
                };
            }

            var channel = _channels[index];
            var cts = new CancellationTokenSource(TimeSpan.FromSeconds(_waitEnqueue));

            try
            {
                if(await channel.Writer.WaitToWriteAsync(cts.Token))
                {
                    if (channel.Writer.TryWrite(model.Data))
                    {
                        return new EnqueueResult
                        {
                            Success = true,
                            Status = "TryWrite success"
                        };
                    }
                   
                }
            }
            catch(OperationCanceledException ex)
            {
                _logger.LogWarning("[{EventId}] {Timestamp} - WaitEnqueue timeout for channel {ChannelId}", ++_eventCounter, DateTime.Now, model.Id);
                return new EnqueueResult
                {
                    Success = false,
                    Status = "WaitEnqueue timout expired"
                };
            }

            return new EnqueueResult
            {
                Success = false,
                Status = "TryWrite failed"
            };
        }
    }

}

