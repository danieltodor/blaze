BINARY = blaze
OBJ_DIR = ./obj
BIN_DIR = ./bin
SRC_DIR = ./src
INCLUDE_DIR = ./include

CXX = g++
CPPFLAGS = -MMD -MP
CXXFLAGS = -I $(INCLUDE_DIR)
LDFLAGS =

SRCS = $(shell find $(SRC_DIR) -name "*.c" -or -name "*.cpp" | sort -k 1nr | cut -f2-)
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst %.o, %.d, $(OBJS))

verbose = false
CMD_PREFIX = @
ifeq ($(verbose),true)
	CMD_PREFIX =
endif

all: $(BIN_DIR)/$(BINARY)

clean:
	@echo "Removing files and directories..."
	$(CMD_PREFIX)rm -r $(OBJ_DIR)
	$(CMD_PREFIX)rm -r $(BIN_DIR)

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

-include $(DEPS)
