#include <iostream>
#include <string>
#include <thread>

#include "boost/asio.hpp"
#include "gateway.hpp"
#include "utils/json_util.hpp"
#include "utils/logger.hpp"

using boost::asio::ip::tcp;

void handle_message(const std::string received_data, tcp::socket& socket, Gateway& gateway,
					boost::system::error_code& error) {
	json request = json_util::parse_json_string(received_data);

	bool is_valid = json_util::validate_request_json(request);

	if (is_valid == false) {
		std::string response = "Fail: request invalid";
		boost::asio::write(socket, boost::asio::buffer(response), error);
	} else {
		std::string response = gateway.process_request(request);
		boost::asio::write(socket, boost::asio::buffer(response), error);
	}
}

void run_server(const std::string& rabbitmq_uri) {
	boost::asio::io_context io_context;

	const uint16_t port = 2400;
	tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
	Gateway gateway(rabbitmq_uri);

	std::cout << "Gateway listening on port " << port << std::endl;

	while (true) {
		tcp::socket socket(io_context);
		acceptor.accept(socket);
		auto client_address = socket.remote_endpoint().address().to_string();

		std::array<char, 1024> data;
		boost::system::error_code error;
		size_t length = socket.read_some(boost::asio::buffer(data), error);
		std::string received_data(data.data(), length);
		logger.log(LogLevel::INFO, "Received request: " + received_data);

		handle_message(received_data, socket, gateway, error);

		socket.close();
	}
}

int main() {
	logger.initialize("logs.txt", LogLevel::INFO);
	std::string rabbitmq_uri = "amqp://guest:guest@rabbitmq:5672/%2F";
	run_server(rabbitmq_uri);

	return 0;
}
