#include <chrono>
#include <cmath>
#include <cstdio>
#include <functional>
#include <iostream>
#include <vector>

#include "data_types.hpp"
#include "engine.hpp"
#include "score_feed.hpp"

int replays = 32 + 1;  // account for warm-up

void feed(Engine& engine, unsigned begin, unsigned end) {
	for (unsigned i = begin; i < end; i++) {
		Order order = rawFeed[i];
		if (order.price == 0) {
			engine.cancel(order.orderId, order.securityId);
		} else {
			engine.limit(rawFeed[i]);
		}
	}
}

unsigned long time(const std::function<void(Engine&, unsigned, unsigned)>& func, Engine& engine,
				   unsigned begin, unsigned end) {
	auto start_t = std::chrono::high_resolution_clock::now();
	func(engine, begin, end);
	auto end_t = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_t - start_t).count();
	return duration;
}

void analytics(std::vector<unsigned long>& samples) {
	unsigned long sumTotal = 0;
	double deviation = 0;

	for (int i = 1; i < replays; i++) {
		sumTotal += samples[i];
	}

	double mean = static_cast<double>(sumTotal) / (replays - 1);
	double meanSqDeviation = 0;

	for (int i = 1; i < replays; i++) {
		deviation = samples[i] - mean;
		meanSqDeviation += (deviation * deviation) / (replays - 1);
	}

	double sd = sqrt(meanSqDeviation);

	printf("latency (mean): %.0f us\n", mean);
	printf("latency (sd): %.0f us\n", sd);
	printf("score: %.0f \n", 0.5 * (mean + sd));
}

int main() {
	int rawFeedLen = sizeof(rawFeed) / sizeof(Order);
	std::vector<unsigned long> samples(replays);

	auto start_t = std::chrono::high_resolution_clock::now();

	for (int j = 0; j < replays; j++) {
		Engine engine;
		auto dur = time(feed, engine, 0, rawFeedLen);
		samples[j] = dur;
	}

	auto end_t = std::chrono::high_resolution_clock::now();
	auto totalDuration =
		std::chrono::duration_cast<std::chrono::milliseconds>(end_t - start_t).count();

	std::cout << "total duration: " << totalDuration << " ms" << std::endl;
	analytics(samples);

	return 0;
}
