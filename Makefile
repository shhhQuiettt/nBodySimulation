# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra 
LDFLAGS = -lraylib -lm -lpthread -ldl -lrt -lX11

# Project name and files
TARGET = nBody
SRC = main.c
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Link objects to create the executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

