using System.Diagnostics;
using System.IO;
using System.Text;

namespace Clearinghouse
{
    public static class Logger
    {
        private static readonly TraceSource traceSource;

        static Logger()
        {
            traceSource = new TraceSource("Clearinghouse");
            traceSource.Listeners.Clear();
            traceSource.Listeners.Add(new TextWriterTraceListener(new StreamWriter("./logs/clearinghouse.log", true, Encoding.UTF8)));
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

        public static void LogInfo(string type, string details)
        {
            string formattedMessage = $"[INFO] type: {type}, details: {details}";
            traceSource.TraceEvent(TraceEventType.Information, 0, formattedMessage);
            ((TextWriterTraceListener)traceSource.Listeners[0]).Flush();
        }
    }
}
