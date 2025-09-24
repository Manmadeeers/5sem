using ResultsAPI.Models;
using System.Text.Json.Serialization;
using Newtonsoft.Json;

namespace ResultsAPI.Service
{
    public interface IResultsService
    {
        Task<IEnumerable<Result>> GetAllResultsAsync();
        Task<Result> GetByKeyAsync(int id);
        Task<Result>AddSync(string value);
        Task<Result> UpdateAsync(int id, string newValue);
        Task<bool> DeleteAsync(int id);
    }
    public class ResultsService:IResultsService
    {
        private static  List<Result> _results;
        private readonly string _filePath = "results.json";
        private readonly object _lock = new();

        public ResultsService()
        {
            LoadResults();
        }

        public void LoadResults()
        {
            if (File.Exists(_filePath))
            {
                var json = File.ReadAllText(_filePath);
                _results = JsonConvert.DeserializeObject<List<Result>>(json) ?? new List<Result>();
            }
            else
            {
                _results = new List<Result>();
            }
        }

        public void SaveResults()
        {
            var json = JsonConvert.SerializeObject(_results, Formatting.Indented);
            File.WriteAllText(_filePath, json);
        }


        public async Task<IEnumerable<Result>> GetAllResultsAsync()
        {
            return await Task.Run(() => _results);
        }

        public async Task<Result>GetByKeyAsync(int id)
        {
            return await Task.Run(() => _results.FirstOrDefault(r => r.Key == id));
        }

        public async Task<Result> AddSync(string value)
        {
            var addedResult = new Result { Key = _results.Count > 0 ? _results.Max(r => r.Key) + 1 : 1, Value = value };

            lock (_lock)
            {
                _results.Add(addedResult);
                SaveResults();
            }

            return await Task.FromResult(addedResult);
        }


        public async Task<Result> UpdateAsync(int id, string newValue)
        {
            Result foundResult =await GetByKeyAsync(id);
            if(foundResult is not null)
            {
                lock (_lock)
                {
                    foundResult.Value = newValue;
                    SaveResults();
                }
            }

            return await Task.FromResult(foundResult);

        }

        public async Task<bool>DeleteAsync(int id)
        {
            Result foundResult = await GetByKeyAsync(id);

            if(foundResult is not null)
            {
                lock (_lock)
                {
                    _results.Remove(foundResult);
                    SaveResults();
                }

                return true;
            }

            return false;
        }
    }
}
