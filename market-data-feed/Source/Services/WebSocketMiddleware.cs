using System;
using System.Collections.Concurrent;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using MarketDataFeed;
using Microsoft.AspNetCore.Http;

public class WebSocketMiddleware
{
    private readonly RequestDelegate _next;
    private IMessageBroadcaster _messageBroadcaster;
    private IHostApplicationLifetime _appLifetime;

    public WebSocketMiddleware(RequestDelegate next, IHostApplicationLifetime appLifetime, IMessageBroadcaster messageBroadcaster)
    {
        _next = next;
        _appLifetime = appLifetime;
        _messageBroadcaster = messageBroadcaster;

        _appLifetime.ApplicationStopping.Register(OnApplicationStopping);
    }

    public async Task InvokeAsync(HttpContext context)
    {
        if (context.Request.Path == "/stream/price")
        {
            if (context.WebSockets.IsWebSocketRequest)
            {
                WebSocket webSocket = await context.WebSockets.AcceptWebSocketAsync();

                var broadcaster = (IMessageBroadcaster)context.RequestServices.GetService(typeof(IMessageBroadcaster))!;

                if (broadcaster == null)
                {
                    throw new Exception("Failed to retrieve IMessageBroadcaster from service collection");
                }

                ((WebSocketMessageBroadcaster)broadcaster).AddWebSocket(webSocket, "price");

                await PriceStream(context, webSocket, _appLifetime.ApplicationStopping);

                ((WebSocketMessageBroadcaster)broadcaster).RemoveWebSocket(webSocket, "price");
            }
            else
            {
                context.Response.StatusCode = 400;
            }
        }
        else if (context.Request.Path == "/stream/report")
        {
            if (context.WebSockets.IsWebSocketRequest)
            {
                WebSocket webSocket = await context.WebSockets.AcceptWebSocketAsync();

                var broadcaster = (IMessageBroadcaster)context.RequestServices.GetService(typeof(IMessageBroadcaster))!;

                if (broadcaster == null)
                {
                    throw new Exception("Failed to retrieve IMessageBroadcaster from service collection");
                }

                ((WebSocketMessageBroadcaster)broadcaster).AddWebSocket(webSocket, "report");

                await ReportStream(context, webSocket, _appLifetime.ApplicationStopping);

                ((WebSocketMessageBroadcaster)broadcaster).RemoveWebSocket(webSocket, "report");
            }
            else
            {
                context.Response.StatusCode = 400;
            }
        }
        else
        {
            await _next(context);
        }
    }

    private async Task PriceStream(HttpContext context, WebSocket webSocket, CancellationToken cancellationToken)
    {
        try
        {
            var buffer = new byte[1024 * 4];
            WebSocketReceiveResult result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), cancellationToken);
            while (!result.CloseStatus.HasValue)
            {
                result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), cancellationToken);
            }
            await webSocket.CloseAsync(result.CloseStatus.Value, result.CloseStatusDescription, cancellationToken);
        }
        catch (OperationCanceledException)
        {

        }
        catch (Exception ex)
        {
            Logger.LogError(ex.ToString());
        }
    }

    private async Task ReportStream(HttpContext context, WebSocket webSocket, CancellationToken cancellationToken)
    {   
        try
        {
            var buffer = new byte[1024 * 4];
            WebSocketReceiveResult result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), cancellationToken);
            while (!result.CloseStatus.HasValue)
            {
                result = await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), cancellationToken);
            }
            await webSocket.CloseAsync(result.CloseStatus.Value, result.CloseStatusDescription, cancellationToken);
        }
        catch(OperationCanceledException)
        {

        }
        catch(Exception ex)
        {
            Logger.LogError(ex.ToString());
        }
    }

    private void OnApplicationStopping()
    {
        if (_messageBroadcaster is WebSocketMessageBroadcaster broadcaster)
        {
            broadcaster.CloseAllSockets();
        }
    }
}