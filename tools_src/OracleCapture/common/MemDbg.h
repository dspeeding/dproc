#ifndef __MEM_DBG_H__
#define __MEM_DBG_H__

#include "basetype.h"

#ifdef  __cplusplus
extern "C" {
#endif

//定义内存记录结构体
typedef struct mem_st
{
    void*			addr;					//分配内存的地址
    unsigned long	num;					//分配内存的大小
    char 			file[MAX_FILE_PATH];	//分配内存当时的源代码文件
    int				line;					//分配内存当时的源代码行号
    unsigned long	threadid;				//分配内存的线程ID
    unsigned long	order;					//第几次内存分配
    time_t			time;					//分配内存时间
} MEM;

typedef MEM* PMEM;
typedef const MEM CMEM;
typedef const MEM* PCMEM;


//记录内存泄露相关信息
typedef struct mem_leak_st 
{
    int		chunks;				//发生了多少次内存泄露
    long	bytes;				//一共有多少字节内存被泄露
} MEM_LEAK;


/*************************************************************************
@Purpose :	申请内存调试
@Param   :	addr	--	申请到的地址
@Param   : ulSize	--	申请内存大小，单位Byte
@Param   : szFile	--	申请内存的源文件地址
@Param   : nLine	--	申请内存的源文件行号
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMemDbg_Malloc(void* addr, unsigned long ulSize, const char* szFile, int nLine);

/*************************************************************************
@Purpose :	释放内存调试
@Param   :	addr	-- 内存地址
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMemDbg_Free(void* addr);

/*************************************************************************
@Purpose :	重新申请内存
@Param   :	addrOld	--	[IN] 原内存地址
@Param   :	addrNew	--	[IN] 新内存地址
@Param   :	ulSize	--	[IN] 申请内存大小，单位Byte
@Param   :	szFile	--	[IN] 申请内存的源文件地址
@Param   :	nLine	--	[IN] 申请内存的源文件行号
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMemDbg_Realloc(void* addrOld, void* addrNew, unsigned long ulSize, const char* szFile, int nLine);

/*************************************************************************
@Purpose :	打印内存泄露信息
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMemDbg_Leak(void);

#ifdef  __cplusplus
}
#endif

#endif


