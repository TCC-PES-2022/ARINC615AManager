include config.mk

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
INCLUDE_PATH := include

TARGET_NAME := libarinc615a.so
TARGET := $(BIN_PATH)/$(TARGET_NAME)

INCDIRS := $(addprefix -I,$(shell find $(INCLUDE_PATH) -type d -print))
INCDIRS += $(addprefix -I,$(DEP_PATH)/include)

# src files & obj files
SRC := $(shell find $(SRC_PATH) -type f -name "*.cpp")
OBJ := $(subst $(SRC_PATH),$(OBJ_PATH),$(SRC:%.cpp=%.o))
OBJDIRS:=$(dir $(OBJ))

# clean files list
DISTCLEAN_LIST := $(OBJ)
CLEAN_LIST := $(DISTCLEAN_LIST)

CLEAN_DEPS := $(DEPS)

# default rule
default: all

# non-phony targets
#$(DEPS): $@
#	@echo "Compiling $<"

TransferManager:
	cd modules/TransferManager && make -j$(shell echo $$((`nproc`))) && make install

cJSON:
	cd modules/cJSON && mkdir -p build && cd build && cmake .. -DCMAKE_INSTALL_PREFIX=$(INSTALL_PATH) \
	&& make -j$(shell echo $$((`nproc`))) && make install

$(TARGET): $(DEPS) $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(LINKFLAGS) $(INCDIRS) $(LDFLAGS) $(LDLIBS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	@echo "Compiling $<"
	$(CXX) $(COBJFLAGS) -o $@ $< $(INCDIRS)

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(OBJDIRS) $(BIN_PATH)

.PHONY: all
all: makedir $(TARGET)

.PHONY: target
target: makedir $(TARGET)

.PHONY: test
test: makedir $(TARGET)

.PHONY: debug
debug: makedir $(TARGET)

.PHONY: install
install:
	mkdir -p $(INSTALL_PATH)/lib $(INSTALL_PATH)/include
	cp -f $(BIN_PATH)/*.so $(INSTALL_PATH)/lib
	cp -f $(shell find $(INCLUDE_PATH) -type f -name "*.h") $(INSTALL_PATH)/include

# TODO: remove only what we installed
.PHONY: uninstall
uninstall:
	rm -rf $(INSTALL_PATH)/lib $(INSTALL_PATH)/include

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
	@rm -rf $(OBJ_PATH)