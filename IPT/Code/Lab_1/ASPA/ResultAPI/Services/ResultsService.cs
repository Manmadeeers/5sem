using ResultAPI.Models;
using System.Text.Json;

namespace ResultAPI.Services
{
    public class ResultsService
    {
        private readonly string _filePath = Path.Combine(Directory.GetCurrentDirectory(), "results.json");
        private readonly object _lockObject = new object();
        public ResultsService()
        {
            if (!File.Exists(_filePath))
            {
                File.WriteAllText(_filePath, "[]");
            }
        }

        private async Task SaveResultsAsync(List<Result> results)
        {
            var json = JsonSerializer.Serialize(results);
            await File.WriteAllTextAsync(_filePath,json);
        }

        public async Task<List<Result>> GetAllAsync()
        {
            var json = await File.ReadAllTextAsync(_filePath);
            return JsonSerializer.Deserialize<List<Result>>(json) ?? new List<Result>();
        }

        public async Task<Result> GetResultAsync(int key)
        {
            var results = await GetAllAsync();
            return results.FirstOrDefault(r => r.Key == key);
        }

        public async Task<bool> AddAsync(Result result)
        {
            var results = await GetAllAsync();
            if(results.FirstOrDefault(r=>r.Key== result.Key) == null)
            {
                results.Add(result);
                await SaveResultsAsync(results);
                return true;
            }
            return false;
        }

        public async Task<bool> UpdateAsync(int key, string updValue)
        {
            var results = await GetAllAsync();
            var resultToUpdate = results.FirstOrDefault(r => r.Key == key);
            if (resultToUpdate != null)
            {
                resultToUpdate.Value = updValue;
                await SaveResultsAsync(results);
                return true;
            }
            return false;
        }

        public async Task<bool>DeleteAsync(int key)
        {
            var results = await GetAllAsync();
            var resultToDelete = results.FirstOrDefault(r => r.Key == key);
            if (resultToDelete != null)
            {
                results.Remove(resultToDelete);
                await SaveResultsAsync(results);
                return true;
            }
            return false;
        }
    }
}
