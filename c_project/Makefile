# ==============================================================================
# Makefile for Smart Food Redistribution System — Professional DSA Edition
# Compiler: GCC (GNU Compiler Collection)
# Standards: C11 (-std=c11) with strict warnings (-Wall -Wextra)
# ==============================================================================

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -pedantic -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE -Iinclude -O2 -g
LDFLAGS := 

SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin
DATA_DIR:= data

TARGET  := $(BIN_DIR)/food_redistribution

SRCS    := $(SRC_DIR)/main.c \
           $(SRC_DIR)/donor.c \
           $(SRC_DIR)/donation.c \
           $(SRC_DIR)/recipient.c \
           $(SRC_DIR)/request_queue.c \
           $(SRC_DIR)/priority_queue.c \
           $(SRC_DIR)/hash_table.c \
           $(SRC_DIR)/allocation.c \
           $(SRC_DIR)/distribution.c \
           $(SRC_DIR)/expiry.c \
           $(SRC_DIR)/analytics.c \
           $(SRC_DIR)/audit.c \
           $(SRC_DIR)/backup.c \
           $(SRC_DIR)/search.c \
           $(SRC_DIR)/demo.c \
           $(SRC_DIR)/utils.c

OBJS    := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean run setup test

all: setup $(TARGET)

setup:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR) $(DATA_DIR)/backups reports

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "--------------------------------------------------------"
	@echo "BUILD SUCCESSFUL: Generated executable at $(TARGET)"
	@echo "Run with: ./$(TARGET)"
	@echo "--------------------------------------------------------"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) reports
	@echo "Clean completed."
