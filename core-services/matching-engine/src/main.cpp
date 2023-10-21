#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <thread>

#include "SimpleAmqpClient/Envelope.h"
#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "engine_operator.hpp"
#include "utils/json_util.hpp"
#include "utils/logger.hpp"

int init_client(const std::string &host, int port) {
	int client_sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server_addr{AF_INET, htons(port)};
	inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
	connect(client_sock, (sockaddr *)&server_addr, sizeof(server_addr));
	return client_sock;
}

void callback(const std::string &body, const std::string &host, int port) {
	logger.log(LogLevel::INFO, "Received message: " + body);

	int client_sock = init_client(host, port);
	send(client_sock, body.c_str(), body.size(), 0);

	char buffer[1024];
	ssize_t response_len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
	buffer[response_len] = '\0';
	std::string response(buffer);

	close(client_sock);
}

void message_listener(const std::string &queue_name, const std::string &rabbitmq_uri,
					  const std::string &host, int port) {
	AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::CreateFromUri(rabbitmq_uri);
	channel->DeclareQueue(queue_name, true);
	channel->BindQueue(queue_name, "stock_exchange", "order_entry");

	std::string consumer_tag = channel->BasicConsume(queue_name, "", false, true, false);

	while (true) {
		AmqpClient::Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer_tag);
		std::string body = envelope->Message()->Body();
		callback(body, host, port);
	}
}

int main() {
	logger.initialize("logs.txt", LogLevel::INFO);
	EngineOperator engine_operator;
	std::thread engine_thread(&EngineOperator::run, &engine_operator);

	message_listener("order_entry", "amqp://guest:guest@rabbitmq:5672/%2F", "localhost", 2401);

	engine_thread.join();
	return 0;
}
