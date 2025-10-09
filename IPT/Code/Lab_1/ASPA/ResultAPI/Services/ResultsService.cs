using ResultAPI.Models;
using System.Text.Json;

namespace ResultAPI.Services
{
    public class ResultsService
    {
        private readonly string _filePath = Path.Combine(Directory.GetCurrentDirectory(), "results.json");
        private readonly SemaphoreSlim _semaphore = new SemaphoreSlim(1,1);
        private readonly object _lockObject = new object();
        public ResultsService()
        {
            if (!File.Exists(_filePath))
            {
                File.WriteAllText(_filePath, "[]");
            }
        }

        private void SaveResultsSync(List<Result> results)
        {
            var json = JsonSerializer.Serialize(results);
            File.WriteAllText(_filePath,json);
        }

        public List<Result> GetAllSync()
        {
            lock (_lockObject)
            {
                var json = File.ReadAllText(_filePath);

                return JsonSerializer.Deserialize<List<Result>>(json) ?? new List<Result>();
            }

         
        }

        public Result GetResultSync(int key)
        {
            lock (_lockObject)
            {
                var results = GetAllSync();
                if (results == null)
                {
                    return null;
                }
                return results.FirstOrDefault(r => r.Key == key);
            }
           
        }

        public async Task<bool> AddSync(Result result)
        {
            lock (_lockObject)
            {
                var results = GetAllSync();
                if (results.FirstOrDefault(r => r.Key == result.Key) == null)
                {
                    results.Add(result);
                    SaveResultsSync(results);
                    return true;
                }
                return false;
            }
          
            
        }

        public async Task<bool> UpdateSync(int key, string updValue)
        {
            lock (_lockObject)
            {
                var results = GetAllSync();
                var resultToUpdate = results.FirstOrDefault(r => r.Key == key);
                if (resultToUpdate != null)
                {
                    resultToUpdate.Value = updValue;
                    SaveResultsSync(results);
                    return true;
                }
                return false;

            }
           
        }

        public async Task<bool>DeleteSync(int key)
        {
            lock (_lockObject)
            {
                var results = GetAllSync();
                var resultToDelete = results.FirstOrDefault(r => r.Key == key);
                if (resultToDelete != null)
                {
                    results.Remove(resultToDelete);
                    SaveResultsSync(results);
                    return true;
                }
                return false;
            }

        }
    }
}