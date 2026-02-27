# Makefile for mysh - Unix Shell Implementation
# CMSC 125 Laboratory Assignment 1

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -pedantic -g -D_POSIX_C_SOURCE=200809L
LDFLAGS =

# Target executable
TARGET = mysh

# Source files
SOURCES = mysh.c tokenizer.c parser.c executor.c
HEADERS = shell.h

# Object files (automatically generated from source files)
OBJECTS = $(SOURCES:.c=.o)

# Default target - builds the shell
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Build successful! Run with ./$(TARGET)"

# Compile source files to object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJECTS)
	@echo "Clean complete"

# Rebuild everything from scratch
rebuild: clean all

# Run the shell (for testing convenience)
run: $(TARGET)
	./$(TARGET)

# Display help information
help:
	@echo "Makefile for mysh - Unix Shell"
	@echo ""
	@echo "Available targets:"
	@echo "  all      - Compile the shell (default)"
	@echo "  clean    - Remove binaries and object files"
	@echo "  rebuild  - Clean and rebuild from scratch"
	@echo "  run      - Build and run the shell"
	@echo "  help     - Display this help message"

# Mark phony targets (targets that don't represent files)
.PHONY: all clean rebuild run help
