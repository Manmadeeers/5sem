using BSTU.Results.Collection.Models;
using System.Text.Json;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using System;
using System.Linq;

namespace BSTU.Results.Collection.Services
{
    public class ResultsService
    {
        private readonly string _filePath = Path.Combine(Directory.GetCurrentDirectory(), "results.json");
        private readonly SemaphoreSlim _semaphore = new SemaphoreSlim(1, 1);
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
            await File.WriteAllTextAsync(_filePath, json);
        }

        public async Task<List<Result>> GetAllAsync()
        {
            await _semaphore.WaitAsync();
            try
            {
                var json = await File.ReadAllTextAsync(_filePath);
                return JsonSerializer.Deserialize<List<Result>>(json) ?? new List<Result>();
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return null;
            }
            finally
            {
                _semaphore.Release();
            }
        }

        public async Task<Result> GetResultAsync(int key)
        {
            var results = await GetAllAsync();
            if (results == null)
            {
                return null;
            }
            return results.FirstOrDefault(r => r.Key == key);
        }

        public async Task<bool> AddAsync(Result result)
        {
            await _semaphore.WaitAsync();
            try
            {
                var results = await GetAllAsync();
                if (results.FirstOrDefault(r => r.Key == result.Key) == null)
                {
                    results.Add(result);
                    await SaveResultsAsync(results);
                    return true;
                }
                return false;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return false;
            }
            finally
            {
                _semaphore.Release();
            }

        }

        public async Task<bool> UpdateAsync(int key, string updValue)
        {
            await _semaphore.WaitAsync();
            try
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
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return false;
            }
            finally
            {
                _semaphore.Release();
            }

        }

        public async Task<bool> DeleteAsync(int key)
        {
            await _semaphore.WaitAsync();
            try
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
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
                return false;
            }
            finally
            {
                _semaphore.Release();
            }

        }
    }
}
