# version
VERSION = 0.1

# paths
DESTDIR 	?= /tmp
DEP_PATH 	?= $(DESTDIR)

DEPS 		:= TransferManager cJSON
LIB_DEPS	:= libtransfer.a libcjson.a

AR 			?= ar
ARFLAGS		:= rcs
CXX 		?=
CXXFLAGS 	:= -Wall -Werror -std=c++11 -pthread
DBGFLAGS 	:= -g -ggdb
TESTFLAGS 	:= -fprofile-arcs -ftest-coverage --coverage

COBJFLAGS 	:= $(CXXFLAGS) -c
test: COBJFLAGS 	+= $(TESTFLAGS)
test: LINKFLAGS 	+= -fprofile-arcs -lgcov
debug: COBJFLAGS 	+= $(DBGFLAGS)