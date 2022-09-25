# version
VERSION = 0.1

# paths
DEST 	:= /opt/fls
DEPS 	:= TransferManager cJSON

INSTALL_PATH 	:= $(DEST)
DEP_PATH 		:= $(DEST)

CXX 		?=
CXXFLAGS 	:= -Wall -Werror -std=c++11 -pthread
DBGFLAGS 	:= -g -ggdb
TESTFLAGS 	:= -fprofile-arcs -ftest-coverage --coverage -lgcov -lgtest -lgcov
LINKFLAGS 	:= -shared
LDFLAGS  	:= -L$(DEP_PATH)/lib
LDLIBS   	:= -ltransfer -ltftp -ltftpd -lpthread -lcjson

COBJFLAGS 	:= $(CXXFLAGS) -c -fPIC
test: COBJFLAGS 	+= $(TESTFLAGS)
test: LINKFLAGS 	+= -lgcov
debug: COBJFLAGS 	+= $(DBGFLAGS)
