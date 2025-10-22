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
            File.WriteAllText(_filePath, json);
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

        public bool AddSync(Result result)
        {
            lock (_lockObject)
            {

                var results = GetAllSync();
                if (result == null)
                {
                    return false;
                }
                if (results.FirstOrDefault(r => r.Key == result.Key) == null)
                {
                    if (result.Value == null || result.Value == string.Empty)
                    {
                        return false;
                    }
                    results.Add(result);
                    SaveResultsSync(results);
                    return true;
                }
                return false;
            }


        }

        public bool UpdateSync(int key, string updValue)
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

        public bool DeleteSync(int key)
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

