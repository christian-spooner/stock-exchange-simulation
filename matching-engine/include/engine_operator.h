#pragma once

#include <string>

#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include "engine.h"

class EngineOperator {
   public:
	void run();

   private:
	Engine engine;
	void handle_order(const std::string& message);
};
