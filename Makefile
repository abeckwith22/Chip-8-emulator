# ============================
# Compiler + Compiler flags
# ============================
CC := gcc
CFLAGS := 
LFLAGS := -I/include

SRC_DIR := src
BUILD_DIR := build

TARGET := app.out

# =====================
# Source files
# =====================
SRCS = $(shell find $(SRC_DIR) -name "*.c")
# =====================
# Object files
# =====================
# This line substitutes the matching filenames from SRCS
# So for instance, SRCS contains `src/main.c` which when
# matched with the line $(SRC_DIR)/%.c it should match with 
# the SRCS string and substitute the match with $(BUILD_DIR)/%.o wildcard. 
# This is why we have the `:` in the front but nowhere else; It's the filename
# string we want to match with when substituting for creating our build targets.
# Saving this for me if I happen to need it and find it one day.
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

# Creates build directory
# =====================
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link executable
# =====================
$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

# C files compiling
# =====================
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $< -o $@ $(LFLAGS)
	
# Clean up
# =====================
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Run
# =====================
.PHONY: run
run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

# Print for debugging
# =====================
.PHONY: print 
print:
	@echo "Source files : $(SRCS)"
	@echo "Object files : $(OBJS)"
