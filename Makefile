# Installation configuration
PREFIX := /usr

# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror

# Define the source files and directories
SRC_DIR = src
INC_DIR = include
EXAMPLES_DIR = examples
BIN_DIR = bin
LIB_DIR = lib

# Define the library and example targets
LIB_NAME = common
LIB_SRC = $(SRC_DIR)/libcommon.c
LIB_OBJ = $(LIB_DIR)/libcommon.o
LIB_TARGET = $(LIB_DIR)/lib$(LIB_NAME).a

EXAMPLES_SRC = $(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLES_OBJ = $(EXAMPLES_SRC:$(EXAMPLES_DIR)/%.c=$(BIN_DIR)/%.o)
EXAMPLES_TARGETS = $(EXAMPLES_SRC:$(EXAMPLES_DIR)/%.c=$(BIN_DIR)/common_%)

# Create directories if they don't exist
$(shell mkdir -p $(BIN_DIR) $(LIB_DIR))

# Default target
all: $(LIB_TARGET) $(EXAMPLES_TARGETS)

# Rule to build the library
$(LIB_TARGET): $(LIB_OBJ)
	ar rcs $@ $^
	@rm $(LIB_DIR)/*.o

$(LIB_OBJ): $(LIB_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build the examples
$(BIN_DIR)/common_%: $(EXAMPLES_DIR)/%.c $(LIB_TARGET)
	$(CC) $(CFLAGS) -c $< -o $(BIN_DIR)/$*.o
	$(CC) $(CFLAGS) -o $@ $(BIN_DIR)/$*.o -L$(LIB_DIR) -l$(LIB_NAME)
	@rm $(BIN_DIR)/$*.o

# Install the built library in the system
install: all
	mkdir -p $(PREFIX)/$(INC_DIR) $(PREFIX)/$(LIB_DIR)
	cp -rf $(INC_DIR)/* $(PREFIX)/$(INC_DIR)
	cp -rf $(LIB_DIR)/* $(PREFIX)/$(LIB_DIR)

$(PREFIX)/$(LIB_DIR)/*.a: install

$(PREFIX)/$(BIN_DIR): $(PREFIX)/$(LIB_DIR)/*.a
	mkdir -p $(PREFIX)/$(BIN_DIR)

install-examples: $(PREFIX)/$(BIN_DIR) $(EXAMPLES_TARGETS)
	cp -rf $(BIN_DIR)/* $(PREFIX)/$(BIN_DIR)

# Clean up
clean:
	rm -rf $(BIN_DIR) $(LIB_DIR)

.PHONY: all clean