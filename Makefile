BINARY = blaze
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
ASM_DIR = $(BUILD_DIR)/asm
BIN_DIR = $(BUILD_DIR)/bin
SRC_DIR = src
INCLUDE_DIRS = include

CXX = g++
CPPFLAGS = -MMD -MP
CXXFLAGS =
CXXFLAGS += $(addprefix -I , $(INCLUDE_DIRS))
LDFLAGS =
OBJDUMPFLAGS = --disassemble --demangle

SRCS = $(shell find $(SRC_DIR) -name "*.c" -or -name "*.cpp" | sort -k 1nr | cut -f2-)
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst %.o, %.d, $(OBJS))

objdump = false
verbose = false
CMD_PREFIX = @
ifeq ($(verbose),true)
	CMD_PREFIX =
endif

all: $(BIN_DIR)/$(BINARY)

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

$(BIN_DIR)/$(BINARY): $(OBJS)
	@echo "Linking..."
	$(CMD_PREFIX)mkdir -p $(BIN_DIR)
	$(CMD_PREFIX)$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%
	@echo "Compiling: $<"
	$(CMD_PREFIX)mkdir -p $(OBJ_DIR)
	$(CMD_PREFIX)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

ifeq ($(objdump),true)
	$(CMD_PREFIX)mkdir -p $(ASM_DIR)
	$(eval filename = $(patsubst $(OBJ_DIR)/%.o, $(ASM_DIR)/%.s, $@))
	$(CMD_PREFIX)touch $(filename)
	$(CMD_PREFIX)objdump $(OBJDUMPFLAGS) $@ > $(filename)
endif

-include $(DEPS)
