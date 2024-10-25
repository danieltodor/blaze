# --- User flags ---
# Echo commands
verbose =
# Create release build
release =
# Run objdump on object files
objdump =

# --- Conditions ---
# Make shell commands visible
ifdef verbose
CMD_PREFIX =
else
CMD_PREFIX = @
endif

# --- Directory structure ---
# Name of the generated binary
BIN_NAME = blaze
# Directories to search for header files
INCLUDE_DIRS = . external
# Directory for the main source code
SRC_DIR = src
BUILD_DIR = build
ifdef release
BUILD_TYPE = release
else
BUILD_TYPE = debug
endif
BIN_DIR = $(BUILD_DIR)/$(BUILD_TYPE)/bin
OBJ_DIR = $(BUILD_DIR)/$(BUILD_TYPE)/obj
OBJ_DUMP_DIR = $(BUILD_DIR)/$(BUILD_TYPE)/dump
BINARY = $(BIN_DIR)/$(BIN_NAME)
SRCS = $(shell find $(SRC_DIR) -name "*.cpp" -or -name "*.c" -or -name "*.s" | sort -k 1nr | cut -f2-)
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst %.o, %.d, $(OBJS))
INSTALL_DIR = ~/.local

# --- Compiler attributes ---
COMPILER = g++
# Language standard
COMPILER_FLAGS = -std=c++17
# Optimisations
ifdef release
COMPILER_FLAGS += -Os -flto=auto
else
COMPILER_FLAGS += -g -O0
endif
# Warnings
ifdef release
COMPILER_FLAGS += -Wno-psabi
else
COMPILER_FLAGS += -Wall -Wextra -Wpedantic -Wshadow
endif
# Instrumentation
ifdef release
COMPILER_FLAGS +=
else
COMPILER_FLAGS += -fsanitize=address,undefined
endif
# Defines
ifdef release
COMPILER_FLAGS += $(addprefix -D, )
else
COMPILER_FLAGS += $(addprefix -D, TEST)
endif
# Includes
COMPILER_FLAGS += $(addprefix -I, $(INCLUDE_DIRS))
# Generate dependency files
COMPILER_FLAGS += -MMD -MP

# --- Linker attributes ---
LINKER = $(COMPILER)
LINKER_FLAGS = $(COMPILER_FLAGS)

# --- Strip attributes ---
STRIP = strip
STRIP_FLAGS = --strip-all

# --- Objdump attributes ---
OBJDUMP = objdump
OBJDUMP_FLAGS = --disassemble --demangle

# --- Make attributes ---
# Disable implicit variables and rules
MAKE_FLAGS = -r -R
# Use multiple threads for faster compilation
MAKE_FLAGS += -j $(shell nproc || echo 2)
MAKEFLAGS = $(MAKE_FLAGS)

# --- Recipes ---
all: $(BINARY)
ifdef release
	@echo "Release build finished"
else
	@echo "Debug build finished"
endif

$(BINARY): $(OBJS)
	@echo "Linking..."
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(LINKER) $(LINKER_FLAGS) $(OBJS) -o $@
ifdef release
	@echo "Stripping binary..."
	$(CMD_PREFIX)$(STRIP) $(STRIP_FLAGS) $(BINARY) -o $(BINARY)
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%
	@echo "Compiling: $<"
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(COMPILER) $(COMPILER_FLAGS) -o $@ -c $<
ifdef objdump
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

test: all
	$(CMD_PREFIX)$(BINARY) --exit

# --- Misc ---
# Include dependencies to know which files need to be recompiled after modification
-include $(DEPS)
