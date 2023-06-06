#!/bin/bash

sleep 10

set -x

# Setup exchange
curl -i -u guest:guest -H 'content-type:application/json' -XPUT -d'{"type":"direct","durable":true}' http://rabbitmq:15672/api/exchanges/%2f/stock_exchange

# Setup queues
curl -i -u guest:guest -H 'content-type:application/json' -XPUT -d'{"durable":true}' http://rabbitmq:15672/api/queues/%2f/order_entry
curl -i -u guest:guest -H 'content-type:application/json' -XPUT -d'{"durable":true}' http://rabbitmq:15672/api/queues/%2f/order_execution
curl -i -u guest:guest -H 'content-type:application/json' -XPUT -d'{"durable":true}' http://rabbitmq:15672/api/queues/%2f/data_feed

# Bind queues to exchange
curl -i -u guest:guest -H 'content-type:application/json' -XPOST -d'{"routing_key":"order_entry"}' http://rabbitmq:15672/api/bindings/%2f/e/stock_exchange/q/order_entry
curl -i -u guest:guest -H 'content-type:application/json' -XPOST -d'{"routing_key":"order_execution"}' http://rabbitmq:15672/api/bindings/%2f/e/stock_exchange/q/order_execution
curl -i -u guest:guest -H 'content-type:application/json' -XPOST -d'{"routing_key":"data_feed"}' http://rabbitmq:15672/api/bindings/%2f/e/stock_exchange/q/data_feed
