# version
VERSION = 0.1

GTEST_ROOT	:=./googletest
DEP_PATH	:= /opt/fls

CXX				?=
CXXFLAGS 		+= -Wall
CXXFLAGS 		+= -Wextra
CXXFLAGS		+= -pthread
CXXFLAGS 		+= -fprofile-arcs -ftest-coverage --coverage
COBJFLAGS 		:= $(CXXFLAGS) -c
LDFLAGS  		:= -L$(GTEST_ROOT)/lib -L$(DEP_PATH)/lib
LDLIBS   		:= -larinc615a -ltransfer -ltftp -ltftpd -lgtest -fprofile-arcs -lgcov -lpthread -lcjson
INCFLAGS 		:= -I$(GTEST_ROOT)/googletest/include -I$(DEP_PATH)/include

#DBGFLAGS 	:= -g -ggdb
#debug: COBJFLAGS 	+= $(DBGFLAGS)
