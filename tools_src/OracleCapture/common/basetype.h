#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "include_stdio.h"
#include "configure.h"
#include "xlog_ex.h"
#include "common.h"
#include "ocvt.h"
#include "xoci.h"
#include "excp.h"
#include "errcode.h"
#include "str_utils.h"
#include "commfunc.h"
#include "Mem.h"

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INANDOUT
#define INANDOUT
#endif

#define CMD_SIGNAL		(SIGRTMIN+4)

//clear buffer
#define BUFCLR(a) memset(a, 0, sizeof(a))

//make var name to string
#define STR(s)  #s

//check null pointer
#define CHECK_POINTER(p, ret) do{\
	if(p==NULL)\
	{\
		LOG_E("%s is null pointer", STR(p));\
	}\
}while(0)

#define LOG_D(fmt, ...) xlog_debug1(fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) xlog_error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_M(fmt, ...) xlog_debug2(fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) xlog_info(fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...) xlog_error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)


