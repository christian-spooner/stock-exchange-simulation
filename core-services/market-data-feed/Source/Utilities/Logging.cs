using System;
using System.IO;
using System.Text;

namespace MarketDataFeed
{
    public static class Logger
    {
        private static readonly string logFilePath = "./logs/market_data_feed.log";

        private static void Log(string level, string message)
        {
            string logEntry = $"[{level}] {message}";
            File.AppendAllText(logFilePath, logEntry + Environment.NewLine);
        }

        public static void LogError(string type, string details)
        {
            string message = $"type: {type}, details: {details}";
            Log("ERROR", message);
        }

        public static void LogWarning(string type, string details)
        {
            string message = $"type: {type}, details: {details}";
            Log("WARNING", message);
        }

        public static void LogInfo(string type, string details)
        {
            string message = $"type: {type}, details: {details}";
            Log("INFO", message);
        }
    }
}
