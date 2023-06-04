CC = g++
CFLAGS = -g -Wall -I./include -I./libs/SimpleAmqpClient -I/usr/include -I./libs/json/include -I./libs/boost
LFLAGS = -L./libs/SimpleAmqpClient/lib -L./libs/boost/stage/lib
LDLIBS = -L/usr/local/lib -lSimpleAmqpClient -lpthread -lboost_system

SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TESTS_DIR = ./tests

SOURCES = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/utils/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

EXECUTABLE = $(BIN_DIR)/main

MAIN_TEST_SUITE = $(TESTS_DIR)/main_test_suite.cpp
TEST_OBJECT = $(MAIN_TEST_SUITE:$(TESTS_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_EXECUTABLE = $(BIN_DIR)/tests

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

tests: $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECT)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LFLAGS) $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS)) $(TEST_OBJECT) -o $(TEST_EXECUTABLE) $(LDLIBS)

$(TEST_OBJECT): $(MAIN_TEST_SUITE)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean tests
