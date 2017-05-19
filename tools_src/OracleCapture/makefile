TARGET = test_cap
CAP_BIN = debug_bin
CC = gcc

SUDE_HOME=${HOME}/sude
INCLUDE=${SUDE_HOME}/include
LIB=${SUDE_HOME}/lib
OBJ_HOME=./obj
OCI_HOME=${HOME}/oci

CFLAGS = -g -D debug -I./inc -I./common -I${INCLUDE} -Wall -Wcast-align -Wwrite-strings -Wpointer-arith -Wstrict-prototypes -Wmissing-prototypes -Wno-uninitialized -DTEST_MEM_ON -D_THREAD_SAFE
LDFLAGS=-lpthread -L${OCI_HOME}/lib -lclntsh -lm

SRC_DIR = ./main ./common

vpath %.c $(SRC_DIR)
vpath %.o $(OBJ_HOME)
SRC_FILES:=$(foreach x,$(SRC_DIR), $(wildcard $(addprefix $(x)/*, .c)))
SRC_FILES:=$(notdir $(SRC_FILES))
OBJ_FILES:=$(addprefix $(OBJ_HOME)/, $(patsubst %.c, %.o, $(SRC_FILES)))

MDLIBS = ${LIB}/libxoci.a	\
		 ${LIB}/libxlog.a	\
		 ${LIB}/libexcp.a	

all:${CAP_BIN}/${TARGET}

build:
	@echo "create directory..."
	-mkdir debug_bin
	-mkdir obj

${CAP_BIN}/${TARGET}:${OBJ_FILES}
	$(CC) ${LDFLAGS} -o $@ $(OBJ_FILES) ${MDLIBS}

${OBJ_HOME}/%.o:%.c
	$(CC) $(CFLAGS) -c $? -o $@

clean:
	find ./ -name \*.o -exec rm {} \;
	-rm -rf ${TARGET}