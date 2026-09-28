CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -Iinclude
SRC_DIR = src
BIN_DIR = bin

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/bmp.c $(SRC_DIR)/stego.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BIN_DIR)/%.o)
TARGET = $(BIN_DIR)/cryptopix

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)

.PHONY: all clean