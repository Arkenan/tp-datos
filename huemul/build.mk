# C++ compiler
CXX = g++
# CXX_FLAGS += -std=c++11 -Wall -pipe
CXX_FLAGS += -std=c++11
# CXX_FLAGS_RELEASE += -O2 -DNDEBUG $(CXX_FLAGS)
# CXX_FLAGS_DEBUG += -O0 -g3 $(CXX_FLAGS)
CXX_FLAGS_RELEASE += -DNDEBUG $(CXX_FLAGS)
CXX_FLAGS_DEBUG += $(CXX_FLAGS)
# CXX_FLAGS_SHARED = -fPIC
# CXX_FLAGS_STATIC = -fPIC

# Linker
LD = $(CXX)
LD_FLAGS += $(CXX_FLAGS)
LD_FLAGS_RELEASE += $(LD_FLAGS)
LD_FLAGS_DEBUG += $(LD_FLAGS)
LD_FLAGS_SHARED = -shared
LD_FLAGS_STATIC =

# Other tools
AR = ar
AR_FLAGS += -rcs
OBJCOPY = objcopy
OBJCOPY_FLAGS += --only-keep-debug
STRIP = strip
STRIP_FLAGS += --strip-debug --strip-unneeded

# Naming conventions
BIN_SUFFIX =
LIB_PREFIX = lib
LIB_SUFFIX_SHARED = .so
LIB_SUFFIX_STATIC = .a
DEBUG_SUFFIX = .debug

# Target file
TARGET ?= unnamed
TARGET_BIN := $(TARGET:$(BIN_SUFFIX)=)$(BIN_SUFFIX)
TARGET_SHARED := $(LIB_PREFIX)$(TARGET:$(LIB_PREFIX)%=%)
TARGET_STATIC := $(TARGET_SHARED)
TARGET_SHARED := $(TARGET_SHARED:$(LIB_SUFFIX_SHARED)=)$(LIB_SUFFIX_SHARED)
TARGET_STATIC := $(TARGET_STATIC:$(LIB_SUFFIX_STATIC)=)$(LIB_SUFFIX_STATIC)
TARGET_BIN_DEBUG := $(TARGET_BIN:$(BIN_SUFFIX)=$(DEBUG_SUFFIX))
TARGET_SHARED_DEBUG := $(TARGET_SHARED:$(LIB_SUFFIX_SHARED)=$(DEBUG_SUFFIX))

# Target type (bin, shared, static)
TARGET_TYPE ?= bin

# Sources and dependencies
SRC_DIR ?= src
SRC_RELEASE += $(SRC)
SRC_DEBUG += $(SRC)
INCLUDE_PATH_RELEASE += $(INCLUDE_PATH)
INCLUDE_PATH_DEBUG += $(INCLUDE_PATH)
LIB_PATH_RELEASE += $(LIB_PATH)
LIB_PATH_DEBUG += $(LIB_PATH)
LIB_RELEASE += $(LIB)
LIB_DEBUG += $(LIB)
DEFINE_RELEASE += $(DEFINE)
DEFINE_DEBUG += $(DEFINE)

# Build directories
RELEASE_DIR = release
DEBUG_DIR = debug

BIN_DIR = bin
LIB_DIR = lib
OBJ_DIR = obj
DEP_DIR = dep
GEN_DIR = gen

OBJ_SRC_DIR = $(OBJ_DIR)/src

BIN_DIR_RELEASE = $(RELEASE_DIR)/$(BIN_DIR)
BIN_DIR_DEBUG = $(DEBUG_DIR)/$(BIN_DIR)
LIB_DIR_RELEASE = $(RELEASE_DIR)/$(LIB_DIR)
LIB_DIR_DEBUG = $(DEBUG_DIR)/$(LIB_DIR)
OBJ_SRC_DIR_RELEASE = $(RELEASE_DIR)/$(OBJ_SRC_DIR)
OBJ_SRC_DIR_DEBUG = $(DEBUG_DIR)/$(OBJ_SRC_DIR)
DEP_DIR_RELEASE = $(RELEASE_DIR)/$(DEP_DIR)
DEP_DIR_DEBUG = $(DEBUG_DIR)/$(DEP_DIR)

# Library-specific compiler options
ifeq ($(TARGET_TYPE), shared)
    CXX_FLAGS += $(CXX_FLAGS_SHARED)
else ifeq ($(TARGET_TYPE), static)
    CXX_FLAGS += $(CXX_FLAGS_STATIC)
endif

CXX_FLAGS_RELEASE += $(INCLUDE_PATH_RELEASE:%=-I%) $(DEFINE_RELEASE:%=-D%)
CXX_FLAGS_DEBUG += $(INCLUDE_PATH_DEBUG:%=-I%) $(DEFINE_DEBUG:%=-D%)
LD_FLAGS_RELEASE += $(LIB_PATH_RELEASE:%=-L%) $(LIB_RELEASE:%=-l%)
LD_FLAGS_DEBUG += $(LIB_PATH_DEBUG:%=-L%) $(LIB_DEBUG:%=-l%)
OBJ_SRC_RELEASE = $(addprefix $(OBJ_SRC_DIR_RELEASE)/, $(SRC_RELEASE:.cpp=.o))
OBJ_SRC_DEBUG = $(addprefix $(OBJ_SRC_DIR_DEBUG)/, $(SRC_DEBUG:.cpp=.o))
DEP_RELEASE = $(addprefix $(DEP_DIR_RELEASE)/, $(SRC_RELEASE:.cpp=.d))
DEP_DEBUG = $(addprefix $(DEP_DIR_DEBUG)/, $(SRC_DEBUG:.cpp=.d))

.PHONY: bin-prepare bin-release bin-debug \
    shared-prepare shared-release shared-debug \
    static-prepare static-release static-debug \
    prepare clean distclean all

all: debug release
release: $(TARGET_TYPE)-release
debug: $(TARGET_TYPE)-debug

clean:
	@rm -f $(OBJ_SRC_RELEASE) $(OBJ_SRC_DEBUG) \
	    $(DEP_RELEASE) $(DEP_DEBUG) \
distclean:
	@rm -rf $(RELEASE_DIR) $(DEBUG_DIR)

prepare:
	@mkdir -p $(sort $(dir $(OBJ_SRC_RELEASE)) $(dir $(OBJ_SRC_DEBUG)) \
	    $(dir $(DEP_RELEASE)) $(dir $(DEP_DEBUG)))

# Executable binary
bin-prepare: prepare
	@mkdir -p $(BIN_DIR_RELEASE) $(BIN_DIR_DEBUG)
bin-release: bin-prepare $(BIN_DIR_RELEASE)/$(TARGET_BIN)
$(BIN_DIR_RELEASE)/$(TARGET_BIN): $(OBJ_SRC_RELEASE)
	$(LD) $^ $(LD_FLAGS_RELEASE) -o $@
	$(OBJCOPY) $(OBJCOPY_FLAGS) $@ $(BIN_DIR_RELEASE)/$(TARGET_BIN_DEBUG)
	@chmod a-x $(BIN_DIR_RELEASE)/$(TARGET_BIN_DEBUG)
	$(STRIP) $(STRIP_FLAGS) $@
bin-debug: bin-prepare $(BIN_DIR_DEBUG)/$(TARGET_BIN)
$(BIN_DIR_DEBUG)/$(TARGET_BIN): $(OBJ_SRC_DEBUG)
	$(LD) $^ $(LD_FLAGS_DEBUG) -o $@

# Shared library
shared-prepare: prepare
	@mkdir -p $(LIB_DIR_RELEASE) $(LIB_DIR_DEBUG)
shared-release: shared-prepare $(LIB_DIR_RELEASE)/$(TARGET_SHARED)
$(LIB_DIR_RELEASE)/$(TARGET_SHARED): $(OBJ_SRC_RELEASE)
	$(LD) $^ $(LD_FLAGS_RELEASE) $(LD_FLAGS_SHARED) -o $@
	$(OBJCOPY) $(OBJCOPY_FLAGS) $@ $(LIB_DIR_RELEASE)/$(TARGET_SHARED_DEBUG)
	@chmod a-x $(LIB_DIR_RELEASE)/$(TARGET_SHARED_DEBUG)
	$(STRIP) $(STRIP_FLAGS) $@
shared-debug: shared-prepare $(LIB_DIR_DEBUG)/$(TARGET_SHARED)
$(LIB_DIR_DEBUG)/$(TARGET_SHARED): $(OBJ_SRC_DEBUG)
	$(LD) $^ $(LD_FLAGS_DEBUG) $(LD_FLAGS_SHARED) -o $@

# Static library
static-prepare: prepare
	@mkdir -p $(LIB_DIR_RELEASE) $(LIB_DIR_DEBUG)
static-release: static-prepare $(LIB_DIR_RELEASE)/$(TARGET_STATIC)
$(LIB_DIR_RELEASE)/$(TARGET_STATIC): $(OBJ_SRC_RELEASE)
	$(AR) $(AR_FLAGS) $@ $^
static-debug: static-prepare $(LIB_DIR_DEBUG)/$(TARGET_STATIC)
$(LIB_DIR_DEBUG)/$(TARGET_STATIC): $(OBJ_SRC_DEBUG)
	$(AR) $(AR_FLAGS) $@ $^

# Object files
$(OBJ_SRC_RELEASE): $(OBJ_SRC_DIR_RELEASE)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS_RELEASE) -MMD -MF $(DEP_DIR_RELEASE)/$(<:$(SRC_DIR)/%.cpp=%.d) -c -o $@ $<
$(OBJ_SRC_DEBUG): $(OBJ_SRC_DIR_DEBUG)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXX_FLAGS_DEBUG) -MMD -MF $(DEP_DIR_DEBUG)/$(<:$(SRC_DIR)/%.cpp=%.d) -c -o $@ $<

-include $(DEP_RELEASE)
-include $(DEP_DEBUG)
