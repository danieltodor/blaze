BINARY = blaze
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
OBJ_DUMP_DIR = $(BUILD_DIR)/dump
BIN_DIR = $(BUILD_DIR)/bin
INSTALL_DIR = ~/.local
SRC_DIR = src
INCLUDE_DIRS = . external

CXX = g++
CPPFLAGS = -MMD -MP
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
CXXFLAGS += $(addprefix -I , $(INCLUDE_DIRS))
LDFLAGS =
OBJDUMPFLAGS = --disassemble --demangle

SRCS = $(shell find $(SRC_DIR) -name "*.c" -or -name "*.cpp" | sort -k 1nr | cut -f2-)
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst %.o, %.d, $(OBJS))

objdump = false
verbose = false
debug = false

CMD_PREFIX = @
ifeq ($(verbose),true)
	CMD_PREFIX =
endif

ifeq ($(debug),true)
	CXXFLAGS += -g
else
	CXXFLAGS += -O3
endif

all: $(BIN_DIR)/$(BINARY)

install:
	@echo "Installing binary to $(INSTALL_DIR)/bin"
	$(CMD_PREFIX)mkdir -p $(INSTALL_DIR)/bin
	$(CMD_PREFIX)cp $(BIN_DIR)/$(BINARY) $(INSTALL_DIR)/bin
	@echo "Installing init scripts to $(INSTALL_DIR)/share/blaze"
	$(CMD_PREFIX)mkdir -p $(INSTALL_DIR)/share/blaze
	$(CMD_PREFIX)cp $(SRC_DIR)/init/* $(INSTALL_DIR)/share/blaze

uninstall:
	@echo "Removing binary from $(INSTALL_DIR)/bin"
	$(CMD_PREFIX)rm -f $(INSTALL_DIR)/bin/$(BINARY)
	@echo "Removing init scripts from $(INSTALL_DIR)/share/blaze"
	$(CMD_PREFIX)rm -rf $(INSTALL_DIR)/share/blaze

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

test:
# The binary must be made with "#define TEST"
	$(CMD_PREFIX)$(BIN_DIR)/$(BINARY) --exit

$(BIN_DIR)/$(BINARY): $(OBJS)
	@echo "Linking..."
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%
	@echo "Compiling: $<"
	$(CMD_PREFIX)mkdir -p $(@D)
	$(CMD_PREFIX)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

ifeq ($(objdump),true)
	$(eval filename = $(patsubst $(OBJ_DIR)/%.o, $(OBJ_DUMP_DIR)/%.dump, $@))
	$(CMD_PREFIX)mkdir -p $(shell dirname $(filename))
	$(CMD_PREFIX)touch $(filename)
	$(CMD_PREFIX)objdump $(OBJDUMPFLAGS) $@ > $(filename)
endif

-include $(DEPS)
