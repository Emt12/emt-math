CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Wpedantic -g3 -O0
CPPFLAGS := -Iinclude
BUILD_DIR := build
TARGET := $(BUILD_DIR)/emt-math
OBJECTS := $(BUILD_DIR)/main.o $(BUILD_DIR)/vector.o

INCLUDE := include/emt
SRC := src
TESTS := tests

VECTOR_I := $(INCLUDE)/vector.h
VECTOR_S := $(SRC)/vector/vector.c
VECTOR_O := $(BUILD_DIR)/vector.o

TEST_SOURCES := $(wildcard tests/test_*.c)
TEST_TARGETS := $(patsubst $(TESTS)/%.c,$(BUILD_DIR)/%,$(TEST_SOURCES))

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@

$(BUILD_DIR)/main.o: $(SRC)/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/vector.o: $(VECTOR_S) $(VECTOR_I) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%: tests/test_%.c $(VECTOR_O) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) $< $(VECTOR_O) -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGETS)
	@set -e; for test_file in $^; do \
		echo "RUNNING $$test_file"; \
		./$$test_file; \
		done

clean:
	$(RM) -r $(BUILD_DIR)
