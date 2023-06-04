using System.Diagnostics;
using System.IO;
using System.Text;

namespace MarketDataFeed
{
    public static class Logger
    {
        private static readonly TraceSource traceSource;

        static Logger()
        {
            traceSource = new TraceSource("MarketDataFeed");
            traceSource.Listeners.Clear();
            traceSource.Listeners.Add(new TextWriterTraceListener(new StreamWriter("logs.txt", true, Encoding.UTF8)));
            traceSource.Switch = new SourceSwitch("sourceSwitch", "Verbose");
        }

        public static void LogError(string message)
        {
            traceSource.TraceEvent(TraceEventType.Error, 2, message);
            ((TextWriterTraceListener)traceSource.Listeners[0]).Flush();
        }

        public static void LogWarning(string message)
        {
            traceSource.TraceEvent(TraceEventType.Warning, 1, message);
            ((TextWriterTraceListener)traceSource.Listeners[0]).Flush();
        }

        public static void LogInfo(string message)
        {
            traceSource.TraceEvent(TraceEventType.Information, 0, message);
            ((TextWriterTraceListener)traceSource.Listeners[0]).Flush();
        }
    }
}
