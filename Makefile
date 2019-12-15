CC := gcc
INCLUDES := include
STORAGE_DIR := data/storage
BIN_DIR := bin
OBJ_DIR := obj
TEST_DIR := test
SRC_DIR := src
SRC_MAIN_DIR := $(SRC_DIR)/main
SRC_TEST_DIR := $(SRC_DIR)/test
DEPS = $(wildcard $(INCLUDES)/*.h)
LINKMATH := -lm
CFLAGS := -I$(INCLUDES) -Wall -g
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SRC_MAIN = $(wildcard $(SRC_MAIN_DIR)/*.c)
OBJ_MAIN = $(SRC_MAIN:$(SRC_MAIN_DIR)/%.c=$(OBJ_DIR)/%.o)
SRC_TEST = $(wildcard $(SRC_TEST_DIR)/*.c)
OBJ_TEST = $(SRC_TEST:$(SRC_TEST_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all
all: main test

.PHONY: main
main: $(OBJ_MAIN)

.PHONY: test
test: $(OBJ_TEST)

$(OBJ_TEST): $(OBJ_DIR)/%.o: $(SRC_TEST_DIR)/%.c $(OBJ) $(DEPS)
	mkdir -p $(TEST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) $(OBJ) $@ -o $(@:$(OBJ_DIR)/%.o=$(TEST_DIR)/%) $(LINKMATH)

$(OBJ_MAIN): $(OBJ_DIR)/%.o: $(SRC_MAIN_DIR)/%.c $(OBJ) $(DEPS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) $(CFLAGS) $(OBJ) $@ -o $(@:$(OBJ_DIR)/%.o=$(BIN_DIR)/%) $(LINKMATH)

$(OBJ): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/* $(TEST_DIR)/* $(STORAGE_DIR)/*

.PHONY: storage_clean
storage_clean:
	rm -rf $(STORAGE_DIR)/*
