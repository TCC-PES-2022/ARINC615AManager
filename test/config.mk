# version
VERSION = 0.1

GTEST_ROOT	:=./googletest
DEP_PATH 	?= $(DESTDIR)

CXX				?=
CXXFLAGS 		+= -Wall
CXXFLAGS 		+= -Wextra
CXXFLAGS		+= -pthread
CXXFLAGS 		+= -fprofile-arcs -ftest-coverage --coverage
COBJFLAGS 		:= $(CXXFLAGS) -c
LDFLAGS  		:= -L$(GTEST_ROOT)/lib -L$(DEP_PATH)/lib
LDLIBS   		:= -larinc615a -ltransfer -ltftp -ltftpd 
LDLIBS 			+= -lgtest -fprofile-arcs -lgcov -lpthread
LDLIBS 			+= $(DEP_PATH)/lib/libcjson.a
INCFLAGS 		:= -I$(GTEST_ROOT)/googletest/include -I$(DEP_PATH)/include

debug: COBJFLAGS 		+= $(DBGFLAGS)
debugdeps: DEP_RULE    	:= debug
testdeps: DEP_RULE    	:= test
