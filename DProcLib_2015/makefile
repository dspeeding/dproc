TARGET = libdproc.a
TARGET_SO = libdproc.so
TARGET_BIN = debug_bin
CC = gcc
AR = ar

INCLUDE= ../comm/include
OBJ_HOME = ./obj
ODBC_HOME = /home/r7/odbc

CFLAGS = -g -D debug -I$(INCLUDE) -I$(ODBC_HOME)/inc -Wall -Wcast-align -fPIC
LDFLAGS = -fPIC -shared

ifeq ($(cover), yes)
CFLAGS += -fprofile-arcs -ftest-coverage
LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif

SRC_DIR = ./ ./src/ds ./src/log ./src/string ./src/mem ./src/xodbc

vpath %.c $(SRC_DIR)
vpath %.o $(OBJ_HOME)
SRC_FILES:=$(foreach x,$(SRC_DIR),$(wildcard $(addprefix $(x)/*, .c)))
SRC_FILES:=$(notdir $(SRC_FILES))
OBJ_FILES:=$(addprefix $(OBJ_HOME)/, $(patsubst %.c,%.o,$(SRC_FILES)))

MDLIBS = 

all:$(TARGET_BIN)/$(TARGET) $(TARGET_BIN)/$(TARGET_SO)



build:
	@echo "create build directory..."
	-mkdir debug_bin
	-mkdir obj

$(TARGET_BIN)/$(TARGET):$(OBJ_FILES)
#	$(CC) $(LDFLAGS) -o $@ $(OBJ_FILES) $(MDLIBS)
	$(AR) -q $(TARGET_BIN)/$(TARGET) $(OBJ_FILES)

$(TARGET_BIN)/$(TARGET_SO):$(OBJ_FILES)
	$(CC) -o $@ $(OBJ_FILES) $(LDFLAGS) $(MDLIBS)

$(OBJ_HOME)/%.o:%.c
	-dos2unix $?
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	find ./ -name \*.o -exec rm {} \;
	-rm -rf $(TARGET_BIN)/$(TARGET_SO)
	-rm -rf $(TARGET_BIN)/$(TARGET)
	-rm -rf ./$(OBJ_HOME)/*.gcda
	-rm -rf ./$(OBJ_HOME)/*.gcno