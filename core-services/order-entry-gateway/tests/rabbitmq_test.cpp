#include <iostream>

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "boost/test/included/unit_test.hpp"

BOOST_AUTO_TEST_CASE(rabbitmq_ping_test) {
	try {
		std::string rabbitmqUri = "amqp://guest:guest@rabbitmq:5672/%2F";
		AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::CreateFromUri(rabbitmqUri);
		std::string exchangeName = "test_exchange";
		std::string queueName = "test_queue";
		std::string routingKey = "test_routing_key";

		channel->DeclareExchange(exchangeName, AmqpClient::Channel::EXCHANGE_TYPE_DIRECT);
		channel->DeclareQueue(queueName, false, true, false, false);
		channel->BindQueue(queueName, exchangeName, routingKey);

		std::string message = "Hello, RabbitMQ!";
		channel->BasicPublish(exchangeName, routingKey, AmqpClient::BasicMessage::Create(message));

		BOOST_CHECK_EQUAL(message, "Hello, RabbitMQ!");
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
