TARGET = testcomm
TARGET_BIN = debug_bin
CC = g++
AR = ar

INCLUDE= ../DProcLib_linux/comm/include
OBJ_HOME = ./obj
LIB_HOME = ../DProcLib_linux/DProcLib_2015/debug_bin
GTEST_INC = /home/r7/gtest/include
GTEST_LIB = /home/r7/gtest
ODBC_HOME = /home/r7/odbc

#git clone https://github.com/google/googletest.git

CFLAGS = -g -D debug -I$(INCLUDE) -I$(GTEST_INC) -I$(ODBC_HOME)/inc -Wall -Wcast-align
LDFLAGS = -lpthread -L$(LIB_HOME) -L$(ODBC_HOME)/lib -ldproc -lodbc

ifeq ($(cover), yes)
CFLAGS += -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov -lodbc
endif

SRC_DIR = ./

vpath %.cpp $(SRC_DIR)
vpath %.o $(OBJ_HOME)
SRC_FILES:=$(foreach x,$(SRC_DIR),$(wildcard $(addprefix $(x)/*, .cpp)))
SRC_FILES:=$(notdir $(SRC_FILES))
OBJ_FILES:=$(addprefix $(OBJ_HOME)/, $(patsubst %.cpp,%.o,$(SRC_FILES)))

MDLIBS = $(GTEST_LIB)/gtest.a

all:$(TARGET_BIN)/$(TARGET)

build:
	@echo "create build directory..."
	-mkdir debug_bin
	-mkdir obj

$(TARGET_BIN)/$(TARGET):$(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $(OBJ_FILES) $(MDLIBS)

$(OBJ_HOME)/%.o:%.cpp
	-dos2unix $?
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	find ./ -name \*.o -exec rm {} \;
	-rm -rf $(TARGET_BIN)/$(TARGET)
	-rm -rf ./$(OBJ_HOME)/*.gcda
	-rm -rf ./$(OBJ_HOME)/*.gcno