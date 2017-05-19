#ifndef __MEM_H__
#define __MEM_H__

#ifdef  __cplusplus
extern "C" {
#endif

#define _MEM								//定义该宏后则实现

#define MEM_STATUS_ON		1				//内存调试打开
#define MEM_STATUS_OFF		0				//内存调试关闭

#ifdef	_MEM

#define Mem_Malloc(size)				MyMem_Malloc(size, __FILE__, __LINE__)
#define Mem_Realloc(addr, size)			MyMem_Realloc(addr, size, __FILE__, __LINE__)
#define Mem_Free(addr)					MyMem_Free(addr)
#define Mem_Leak()						MyMem_Leak()

#else

#define Mem_Malloc(size)				malloc(size)
#define Mem_Realloc(addr, size)			realloc(addr, size)
#define Mem_Free(addr)					free(addr)
#define Mem_Leak()						
#endif


/*************************************************************************
@Purpose :	设置内存调试状态
@Param   :
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMem_SetDbg(int nState);

/*************************************************************************
@Purpose :	打开内存调试
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :默认状态下为关闭内存调试状态
 *************************************************************************/
#define Mem_SetDbgOn()		MyMem_SetDbg(MEM_STATUS_ON)

/*************************************************************************
@Purpose :	关闭内存调试
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
#define Mem_SetDbgOff()		MyMem_SetDbg(MEM_STATUS_OFF)

/*************************************************************************
@Purpose :	申请内存
@Param   :	ulSize	--	申请内存大小，单位Byte
@Param   :	szFile	--	申请内存的源文件地址
@Param   :	nLine	--	申请内存的源文件行号
@Return  :	成功返回申请后内存地址，失败返回NULL
@Modify  :
@Remark  :
 *************************************************************************/
void* MyMem_Malloc(IN unsigned long ulSize, IN const char* szFile, IN int nLine);

/*************************************************************************
@Purpose :	释放申请的内存
@Param   :	addr	--	由Mem_Malloc返回的内存地址
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMem_Free(IN void* addr);

/*************************************************************************
@Purpose :	重新申请内存
@Param   :	addr	--	原内存地址
@Param   :	dwSize	--	申请内存大小，单位Byte
@Param   :	szFile	--	申请内存的源文件地址
@Param   :	nLine	--	申请内存的源文件行号
@Return  :	成功返回新申请的内存地址，失败返回NULL
@Modify  :
@Remark  :
*************************************************************************/
void* MyMem_Realloc(IN void* addr, IN unsigned long ulSize, IN const char* szFile, IN int nLine);

/*************************************************************************
@Purpose :	打印内存泄露信息
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMem_Leak();

#ifdef  __cplusplus
}
#endif

#endif

