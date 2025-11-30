# --- User flags ---
# Echo commands
verbose =
# Create release build
release =
# Create analysis during/after compilation
analyze =

# --- Conditions ---
# Make shell commands visible
ifdef verbose
CMD_PREFIX =
else
CMD_PREFIX = @
endif

# --- Project structure ---
# Name of the generated binary
BIN_NAME = blaze
# Directories to search for header files
INCLUDE_DIRS = src external
# Suppress warnings for these directories
SYSTEM_DIRS = external
# Libraries to link
LIBRARIES =
# Directory for the main source code
SRC_DIR = src
BUILD_DIR = build
ifdef release
BUILD_TYPE = release
else
BUILD_TYPE = debug
endif
TYPE_DIR = $(BUILD_DIR)/$(BUILD_TYPE)
BIN_DIR = $(TYPE_DIR)/bin
OBJ_DIR = $(TYPE_DIR)/obj
BINARY = $(BIN_DIR)/$(BIN_NAME)
SRCS = $(shell find $(SRC_DIR) -name "*.cpp" -or -name "*.c" -or -name "*.s" | sort -k 1nr | cut -f2-)
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst %.o, %.d, $(OBJS))
INSTALL_DIR = ~/.local

# --- Compiler attributes ---
# Compiler
COMPILER_FLAGS = g++
# Language standard
COMPILER_FLAGS += -std=c++17
# Optimisations
ifdef release
COMPILER_FLAGS += -Os -flto=auto
else
COMPILER_FLAGS += -O0 -g3
endif
# Warnings
ifdef release
COMPILER_FLAGS += -Wno-psabi -Wno-deprecated
else
COMPILER_FLAGS += -Wall -Wextra -Wpedantic -Wshadow -Wdouble-promotion -Wfloat-equal -Wcast-qual -Wimplicit-fallthrough \
				  -Wuseless-cast -Wredundant-decls -Wold-style-cast -Wcast-align -Woverloaded-virtual -Wformat=2 \
				  -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wnon-virtual-dtor
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
COMPILER_FLAGS += $(addprefix -isystem, $(SYSTEM_DIRS))
# Libraries
COMPILER_FLAGS += $(addprefix -l, $(LIBRARIES))
# Generate dependency files
COMPILER_FLAGS += -MMD -MP

# --- Linker attributes ---
LINKER_FLAGS = $(COMPILER_FLAGS)

# --- Stripper attributes ---
STRIPPER_FLAGS = strip --strip-all

# --- Analysis attributes ---
FILE_FLAGS = file
LDD_FLAGS = ldd
HEXDUMP_FLAGS = hexdump --canonical
STRINGS_FLAGS = strings
READELF_FLAGS = readelf --all
NM_FLAGS = nm
SIZE_FLAGS = size
OBJDUMP_FLAGS = objdump --disassemble --demangle --source-comment --wide --visualize-jumps --decompress --no-addresses \
				--no-show-raw-insn

# --- Make attributes ---
# Disable implicit variables and rules
MAKE_FLAGS = -r -R
# Use multiple threads for faster compilation
MAKE_FLAGS += -j $(shell nproc 2>/dev/null || echo 4)
MAKEFLAGS = $(MAKE_FLAGS)

# --- Recipes ---
all: $(BINARY)
ifdef release
	@echo "Release build finished"
else
	@echo "Debug build finished"
endif
ifdef analyze
	$(CMD_PREFIX)$(FILE_FLAGS) $< > $<.file
	$(CMD_PREFIX)$(LDD_FLAGS) $< > $<.ldd
	$(CMD_PREFIX)$(HEXDUMP_FLAGS) $< > $<.hexdump
	$(CMD_PREFIX)$(STRINGS_FLAGS) $< > $<.strings
	$(CMD_PREFIX)$(READELF_FLAGS) $< > $<.readelf
	$(CMD_PREFIX)$(NM_FLAGS) $< > $<.nm
	$(CMD_PREFIX)$(SIZE_FLAGS) $< > $<.size
endif

$(BINARY): $(OBJS)
	@echo "Linking..."
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(LINKER_FLAGS) $^ -o $@
ifdef release
	@echo "Stripping binary..."
	$(CMD_PREFIX)$(STRIPPER_FLAGS) $@ -o $@
endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%
	@echo "Compiling: $<"
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(COMPILER_FLAGS) -o $@ -c $<
ifdef analyze
	$(CMD_PREFIX)$(OBJDUMP_FLAGS) $@ > $@.objdump
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
