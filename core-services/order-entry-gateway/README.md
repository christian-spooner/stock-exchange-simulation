# Gateway Specification

### Requests
- Limit Order Request
```
{
  "type": "limit",
  "security_id": "string",
  "client_id": "string",
  "side": "buy",
  "quantity": integer,
  "price": float
}
```
- Cancel Order Request
```
{
    "type": "cancel",
    "order_id": integer,
    "security_id": "string"
} 
```
**Fields**
- `type`: "limit", "cancel"
- `side`: "buy", "sell"

### Responses
- Limit Order Request (Success)
```
{
  "status": "success",
  "order_id": integer,
  "message": "order created"
}
```
- Limit Order Request (Failure)
```
{
  "status": "failure",
  "message": "string"
}
```
- Cancel Order Request (Success)
```
{
    "status": "success",
    "message": "order cancelled"
}
```
- Cancel Order Request (Failure)
```
{
    "status": "failure",
    "message": "string"
}
```
