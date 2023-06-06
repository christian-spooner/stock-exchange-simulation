# Compiler flags
CFLAGS = -Wall -g -I./include -I./libs/json/include -I./libs/SimpleAmqpClient -I/usr/include
LDFLAGS = -L./libs/SimpleAmqpClient/lib -L/usr/local/lib
LDLIBS = -lSimpleAmqpClient -lpthread

# Source files
SRCS = $(wildcard src/*.cpp src/utils/*.cpp)

# Object files
MAIN_OBJS = $(SRCS:src/%.cpp=obj/%.o)
TEST_OBJS = $(SRCS:src/%.cpp=obj_t/%.o)

# Targets
all: directories bin/main

tests: test_units test_engine test_perf

directories: 
	mkdir -p obj bin obj/utils

bin/main: $(MAIN_OBJS)
	g++ $(LDFLAGS) -o bin/main $(MAIN_OBJS) $(LDLIBS)

obj/%.o: src/%.cpp
	g++ $(CFLAGS) -c $< -o $@

test_units: ./bin/test_units

./bin/test_units: obj_t/test_units.o $(filter-out obj_t/main.o,$(TEST_OBJS))
	g++ $(LDFLAGS) -o ./bin/test_units obj_t/test_units.o $(filter-out obj_t/main.o,$(TEST_OBJS)) $(LDLIBS)

test_engine: ./bin/test_engine

./bin/test_engine: obj_t/test_engine.o $(filter-out obj_t/main.o,$(TEST_OBJS))
	g++ $(LDFLAGS) -o ./bin/test_engine obj_t/test_engine.o $(filter-out obj_t/main.o,$(TEST_OBJS)) $(LDLIBS)

test_perf: ./bin/test_perf

./bin/test_perf: obj_t/test_perf.o $(filter-out obj_t/main.o,$(TEST_OBJS))
	g++ $(LDFLAGS) -o ./bin/test_perf obj_t/test_perf.o $(filter-out obj_t/main.o,$(TEST_OBJS)) $(LDLIBS)

obj_t/%.o: src/%.cpp
	g++ -D_DEBUG $(CFLAGS) -c $< -o $@

obj_t/test_units.o: tests/test_units.cpp
	g++ -D_DEBUG $(CFLAGS) -c $< -o $@

obj_t/test_engine.o: tests/test_engine.cpp
	g++ -D_DEBUG $(CFLAGS) -c $< -o $@

obj_t/test_perf.o: tests/test_perf.cpp
	g++ -D_DEBUG $(CFLAGS) -c $< -o $@

clean:
	rm -f ./bin/main ./bin/test_limit_tree ./bin/test_engine ./bin/test_perf *~
	find ./obj -name "*.o" -type f -delete
	find ./obj_t -name "*.o" -type f -delete

.PHONY: all tests clean
