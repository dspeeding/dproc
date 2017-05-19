#ifndef __BASETYPE_H__
#define __BASETYPE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WINDOWS
#pragma   warning(disable:4996)
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#endif


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INANDOUT
#define INANDOUT
#endif

//clear buffer
#define	BUFCLR(a)	memset(a,0,sizeof(a))

//make var name to string
#define STR(s)		#s

//check null pointer
#define CHECK_POINTER(p, ret) do{\
	if(p == NULL)\
	{\
		LOG_E("%s is null pointer", STR(p));\
	}\
}while(0)

//visit function 
typedef int(*VisitPTR)(void*);
//compare function
typedef int(*ComparePTR)(void*, void*);

#include "errcode.h"
#include "log.h"
#include "commfunc.h"
#include "str_utils.h"
#include "str_base64opt.h"
#include "ds/array_stack.h"
#include "ds/sq_list.h"
#include "ds/link_list.h"
#include "ds/array_queue.h"
#include "ds/Link_Hash.h"
#include "mem/Mem.h"


#endif // __BASETYPE_H__

