include config.mk

# path macros
BIN_PATH := bin
SRC_PATH := src
OBJ_PATH := obj
INCLUDE_PATH := include
REPORT_PATH := report

DEPS := arinc615amanager

# compile macros
TARGET_NAME := unity_test_arinc615a
TARGET := $(BIN_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(shell find $(SRC_PATH) -type f -name "*.cpp")
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
CLEAN_LIST := $(OBJ) 			 \
			  $(BIN_PATH)/*		 \
			  $(TARGET) 		 \
			  *.txt 			 \
			  $(OBJ_PATH)/*.gcov \
			  $(OBJ_PATH)/*.gcno \
			  $(OBJ_PATH)/*.gcda \
			  $(SRC_PATH)/*.gcov \
			  $(SRC_PATH)/*.gcno \
			  $(SRC_PATH)/*.gcda \
			  $(DISTCLEAN_LIST)

# default rule
default: all

# non-phony targets
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) \
	$(LINKFLAGS) $(INCFLAGS) $(LDFLAGS) $(LDLIBS)

arinc615amanager:
	cd .. && $(MAKE) $(DEP_RULE) -j$(shell echo $$((`nproc`))) && \
	$(MAKE) install DESTDIR=$(DEP_PATH)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CXX) $(COBJFLAGS) -o $@ $< $(INCFLAGS)

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH)

.PHONY: gtest
gtest:
	cd $(GTEST_ROOT) && cmake --build . -j $(shell echo $$((`nproc`)))

.PHONY: deps
deps: $(DEPS)

.PHONY: debugdeps
debugdeps: $(DEPS)

.PHONY: testdeps
testdeps: $(DEPS)

.PHONY: all
all: makedir $(TARGET)

.PHONY: debug
debug: makedir $(TARGET)

.PHONY: runtests
runtests:
	LD_LIBRARY_PATH=$(DEP_PATH)/lib ./$(TARGET)

.PHONY: report
report:
	cd .. && $(MAKE) clean && cd -
	$(MAKE) clean
	$(MAKE) testdeps
	$(MAKE) all
	$(MAKE) runtests
	@mkdir -p $(REPORT_PATH)
	find ../src -name "*.gcov" -exec gcov {} --object-directory ../obj \;
	lcov --capture --directory ../obj --output-file $(REPORT_PATH)/coverage.info
	genhtml $(REPORT_PATH)/coverage.info --output-directory $(REPORT_PATH)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -rf $(CLEAN_LIST)