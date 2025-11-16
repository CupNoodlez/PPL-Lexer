# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = Lexer
SRCDIR = src
OBJDIR = obj

# Source files
SOURCES = Lexer.c helpers.c
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)
HEADERS = $(SRCDIR)/helpers.h

# Default target
all: $(TARGET)

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Build target executable
$(TARGET): $(OBJDIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRCDIR) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Rebuild everything
rebuild: clean all

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG
debug: $(TARGET)

# Release build (optimized)
release: CFLAGS += -O2 -DNDEBUG
release: $(TARGET)

# Install dependencies (if needed)
install:
	@echo "No external dependencies to install"

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the lexer (default)"
	@echo "  clean    - Remove build files"
	@echo "  rebuild  - Clean and build"
	@echo "  run      - Build and run the lexer"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized version"
	@echo "  help     - Show this help"

.PHONY: all clean rebuild run debug release install help