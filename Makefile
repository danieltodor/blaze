# --- User flags ---
# Default off value
off = 0
# Default on value
on = 1
# Echo commands
verbose = $(off)
# Make binary debuggable
debug = $(off)
# Run objdump on object files
objdump = $(off)

# --- Directory structure ---
# Name of the generated binary
BIN_NAME = blaze
# Directories to search for header files
INCLUDE_DIRS = . external
# Directory for the main source code
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
OBJ_DUMP_DIR = $(BUILD_DIR)/dump
BINARY = $(BIN_DIR)/$(BIN_NAME)
SRCS = $(shell find $(SRC_DIR) -name "*.s" -or -name "*.c" -or -name "*.cpp" | sort -k 1nr | cut -f2-)
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst %.o, %.d, $(OBJS))
INSTALL_DIR = ~/.local

# --- Build attributes ---
COMPILER = g++
# Language standard
COMPILER_FLAGS = -std=c++17
# Optimisations
COMPILER_FLAGS += -Os -flto=auto
# Includes
COMPILER_FLAGS += $(addprefix -I, $(INCLUDE_DIRS))
# Defines
COMPILER_FLAGS += $(addprefix -D, )
# Warnings
COMPILER_FLAGS += -Wno-psabi -Wall -Wextra -Wpedantic -Wshadow
# Generate dependency files
COMPILER_FLAGS += -MMD -MP
LINKER = $(COMPILER)
LINKER_FLAGS = $(COMPILER_FLAGS)
STRIP = strip
STRIP_FLAGS = --strip-all
OBJDUMP = objdump
OBJDUMP_FLAGS = --disassemble --demangle

# --- Make attributes ---
# Disable implicit variables and rules
MAKE_FLAGS = -r -R
# Use multiple threads for faster compilation
MAKE_FLAGS += -j $(shell nproc || echo 2)
MAKEFLAGS = $(MAKE_FLAGS)

# --- Conditions ---
# Make shell commands visible
CMD_PREFIX = @
ifneq ($(verbose), $(off))
	CMD_PREFIX =
endif

# Add debug symbols and disable optimisation
ifneq ($(debug), $(off))
	COMPILER_FLAGS += -g -O0 -fno-lto
endif

# --- Recipes ---
all: $(BINARY)

$(BINARY): $(OBJS)
	@echo "Linking..."
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(LINKER) $(LINKER_FLAGS) $(OBJS) -o $@
ifeq ($(debug), $(off))
	@echo "Stripping binary..."
	$(CMD_PREFIX)$(STRIP) $(STRIP_FLAGS) $(BINARY) -o $(BINARY)
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%
	@echo "Compiling: $<"
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(COMPILER) $(COMPILER_FLAGS) -o $@ -c $<
ifneq ($(objdump), $(off))
	$(eval filename = $(patsubst $(OBJ_DIR)/%.o, $(OBJ_DUMP_DIR)/%.dump, $@))
	$(CMD_PREFIX)mkdir -p $(shell dirname $(filename))
	$(CMD_PREFIX)touch $(filename)
	$(CMD_PREFIX)$(OBJDUMP) $(OBJDUMP_FLAGS) $@ > $(filename)
endif

clean:
	@echo "Removing files and directories..."
	$(CMD_PREFIX)rm -rf $(BUILD_DIR)

info:
	@echo "Source files:"
	@echo $(SRCS)
	@echo
	@echo "Object files:"
	@echo $(OBJS)
	@echo
	@echo "Dependencies:"
	@echo $(DEPS)

install: all
	@echo "Installing binary to $(INSTALL_DIR)/bin"
	$(CMD_PREFIX)mkdir -p $(INSTALL_DIR)/bin
	$(CMD_PREFIX)cp $(BINARY) $(INSTALL_DIR)/bin
	@echo "Installing init scripts to $(INSTALL_DIR)/share/blaze"
	$(CMD_PREFIX)mkdir -p $(INSTALL_DIR)/share/blaze
	$(CMD_PREFIX)cp $(SRC_DIR)/init/* $(INSTALL_DIR)/share/blaze

uninstall:
	@echo "Removing binary from $(INSTALL_DIR)/bin"
	$(CMD_PREFIX)rm -f $(INSTALL_DIR)/bin/$(BIN_NAME)
	@echo "Removing init scripts from $(INSTALL_DIR)/share/blaze"
	$(CMD_PREFIX)rm -rf $(INSTALL_DIR)/share/blaze

test: COMPILER_FLAGS += -DTEST
test: all
	$(CMD_PREFIX)$(BINARY) --exit

# --- Misc ---
# Include dependencies to know which files need to be recompiled after modification
-include $(DEPS)
