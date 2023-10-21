#include "engine_operator.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "utils/json_util.hpp"

// Helper functions

int init_server(const std::string &host, const int port) {
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in server_addr{AF_INET, htons(port)};
	inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
	bind(server_sock, (sockaddr *)&server_addr, sizeof(server_addr));
	listen(server_sock, 5);
	std::cout << "Engine listening on port " << port << std::endl;
	return server_sock;
}

// Member functions

void EngineOperator::handle_order(const std::string &message) {
	json orderJson = json_util::parse_json_string(message);

	if (orderJson["type"] == "limit") {
		Order order(orderJson["order_id"].get<OrderId>(),
					orderJson["security_id"].get<std::string>(),
					orderJson["client_id"].get<std::string>(),
					(orderJson["side"].get<std::string>() == "buy") ? BUY : !BUY,
					orderJson["quantity"].get<Quantity>(), orderJson["price"].get<Price>());
		engine.limit(order);
	} else if (orderJson["type"] == "market") {
		Order order(orderJson["order_id"].get<OrderId>(),
					orderJson["security_id"].get<std::string>(),
					orderJson["client_id"].get<std::string>(),
					(orderJson["side"].get<std::string>() == "buy") ? BUY : !BUY,
					orderJson["quantity"].get<Quantity>(), orderJson["price"].get<Price>());
		engine.market(order);
	} else if (orderJson["type"] == "cancel") {
		OrderId orderId = orderJson["order_id"].get<OrderId>();
		std::string securityId = orderJson["security_id"].get<std::string>();
		engine.cancel(orderId, securityId);
	}
}

void EngineOperator::run() {
	int server_sock = init_server("localhost", 2401);

	while (true) {
		sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int client_sock = accept(server_sock, (sockaddr *)&client_addr, &client_addr_len);

		char buffer[1024];
		ssize_t message_len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
		buffer[message_len] = '\0';
		std::string message(buffer);

		send(client_sock, "received", 8, 0);
		close(client_sock);

		handle_order(message);
	}

	close(server_sock);
}
