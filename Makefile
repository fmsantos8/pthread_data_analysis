CC = gcc
CFLAGS = -Wall -g -I./src

SRC_DIR := src
BUILD_DIR := build
BIN := $(BUILD_DIR)/app

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

all: clean $(BIN) run

$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)

run:
	@BUILD_DIR=$(BUILD_DIR) $(BIN)