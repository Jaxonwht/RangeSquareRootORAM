CC := gcc
INCLUDES := include
BIN_DIR := bin
OBJ_DIR := obj
SRC_DIR := src
DATA_DIR := $(CURDIR)/data
SRC_MAIN_DIR := $(SRC_DIR)/main
DEPS = $(wildcard $(INCLUDES)/*.h)
CFLAGS := -I$(INCLUDES) -Wall -DDATA_DIR='"$(DATA_DIR)"'
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SRC_MAIN = $(wildcard $(SRC_MAIN_DIR)/*.c)
OBJ_MAIN = $(SRC_MAIN:$(SRC_MAIN_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all
all: $(OBJ_MAIN)

$(OBJ_MAIN): $(OBJ_DIR)/%.o: $(SRC_MAIN_DIR)/%.c $(OBJ) $(DEPS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) $(OBJ) $@ -o $(@:$(OBJ_DIR)/%.o=$(BIN_DIR)/%)

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(DATA_DIR)/*
