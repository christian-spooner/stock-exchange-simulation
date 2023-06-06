using System.Collections.Concurrent;
using System.Net.WebSockets;
using System.Text;

public interface IMessageBroadcaster
{
    Task BroadcastMessageAsync(string message, string messageType, CancellationToken cancellationToken);
}

public class WebSocketMessageBroadcaster : IMessageBroadcaster
{
    private static ConcurrentDictionary<WebSocket, byte> _priceSockets = new ConcurrentDictionary<WebSocket, byte>();
    private static ConcurrentDictionary<WebSocket, byte> _reportSockets = new ConcurrentDictionary<WebSocket, byte>();

    public void AddWebSocket(WebSocket webSocket, string messageType)
    {
        if (messageType == "price")
        {
            _priceSockets.TryAdd(webSocket, 0);
        }
        else if (messageType == "report")
        {
            _reportSockets.TryAdd(webSocket, 0);
        }
        else
        {
            throw new Exception($"Unknown message type: {messageType}");
        }
    }

    public void RemoveWebSocket(WebSocket webSocket, string messageType)
    {
        if (messageType == "price")
        {
            _priceSockets.TryRemove(webSocket, out _);
        }
        else if (messageType == "report")
        {
            _reportSockets.TryRemove(webSocket, out _);
        }
        else
        {
            throw new Exception($"Unknown message type: {messageType}");
        }
    }

    public async Task BroadcastMessageAsync(string message, string messageType, CancellationToken cancellationToken)
    {
        var buffer = Encoding.UTF8.GetBytes(message);
        ConcurrentDictionary<WebSocket, byte> sockets;

        if (messageType == "price")
        {
            sockets = _priceSockets;
        }
        else if (messageType == "report")
        {
            sockets = _reportSockets;
        }
        else
        {
            throw new Exception($"Unknown message type: {messageType}");
        }

        foreach (var socket in sockets.Keys)
        {
            if (socket.State == WebSocketState.Open)
            {
                await socket.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Text, endOfMessage: true, cancellationToken: cancellationToken);
            }
        }
    }

    public void CloseAllSockets()
    {
        foreach (var socket in _priceSockets.Keys)
        {
            if (socket.State == WebSocketState.Open)
            {
                socket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Application is shutting down", CancellationToken.None).GetAwaiter().GetResult();
            }
        }
        foreach (var socket in _reportSockets.Keys)
        {
            if (socket.State == WebSocketState.Open)
            {
                socket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Application is shutting down", CancellationToken.None).GetAwaiter().GetResult();
            }
        }
    }
}
