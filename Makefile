include config.mk

# path macros
OUT_PATH := lib
OBJ_PATH := obj
SRC_PATH := src
INCLUDE_PATH := include

TARGET_NAME := libarinc615a.a
TARGET := $(OUT_PATH)/$(TARGET_NAME)

INCDIRS := $(addprefix -I,$(shell find $(INCLUDE_PATH) -type d -print))
INCDIRS += $(addprefix -I,$(DEP_PATH)/include)

# src files & obj files
SRC := $(shell find $(SRC_PATH) -type f -name "*.cpp")
OBJ := $(subst $(SRC_PATH),$(OBJ_PATH),$(SRC:%.cpp=%.o))
OBJDIRS:=$(dir $(OBJ))

# clean files list
CLEAN_LIST := $(OBJ) 			\
			  $(OBJ_PATH) 		

# default rule
default: all

$(DEPS): $@
	@echo "\n\n *** Building $@ *** \n\n"
	cd modules/$@ && $(MAKE) deps && \
	$(MAKE) $(DEP_RULE) -j$(shell echo $$((`nproc`))) && \
	$(MAKE) install DESTDIR=$(DEP_PATH)

# LIB_DEPS_COMPLETE := $(addprefix $(DEP_PATH)/lib/,$(LIB_DEPS))
$(TARGET): $(OBJ)
	@echo "Linking $@"
	$(AR) $(ARFLAGS) $(TARGET) $(OBJ)
	# $(AR) $(ARFLAGS) libtmp.a $(OBJ)
	# echo "create $@" > lib.mri
	# echo "addlib libtmp.a" >> lib.mri
	# echo "$(addprefix \naddlib ,$(LIB_DEPS_COMPLETE))" >> lib.mri
	# echo "save" >> lib.mri
	# echo "end" >> lib.mri
	# $(AR) -M < lib.mri
	# rm libtmp.a
	# rm lib.mri
	# ranlib $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	@echo "Building $<"
	$(CXX) $(COBJFLAGS) -o $@ $< $(INCDIRS)

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(OBJDIRS) $(OUT_PATH)

.PHONY: deps
deps: $(DEPS)

.PHONY: all
all: makedir $(TARGET)
	strip --strip-unneeded $(TARGET)
	ranlib $(TARGET)

.PHONY: test
test: makedir $(TARGET)

.PHONY: debug
debug: makedir $(TARGET)

.PHONY: install
install:
	@echo "\n\n *** Installing ARINC615AManager to $(DESTDIR) *** \n\n"
	mkdir -p $(DESTDIR)/lib $(DESTDIR)/include
	cp -f $(TARGET) $(DESTDIR)/lib
	cp -f $(shell find $(INCLUDE_PATH) -type f -name "*.h") $(DESTDIR)/include

# TODO: create uninstall rule

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -rf $(CLEAN_LIST)