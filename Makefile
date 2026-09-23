CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Wpedantic -g3 -O0
BUILD_DIR := build
TARGET := $(BUILD_DIR)/emt-math
SOURCE := src/main.c
OBJECT := $(BUILD_DIR)/main.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECT)
	$(CC) $^ -o $@

$(OBJECT): $(SOURCE) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) -r $(BUILD_DIR)
