#include "basetype.h"
#include "Mem.h"
#include "MemDbg.h"

//当前状态为未调用dbg等函数
int		g_nMemDbgStatus = MEM_STATUS_OFF;		

/*************************************************************************
@Purpose :	设置内存调试状态
@Param   :
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMem_SetDbg(int nState)
{
	switch (nState)
	{
	case MEM_STATUS_OFF:
	case MEM_STATUS_ON:
		g_nMemDbgStatus = nState;
		break;
	default:
		LOG_E("Mem_SetDbg nState err[%d]", nState);
	}
}
	
/*************************************************************************
@Purpose :	申请内存
@Param   :	ulSize	--	申请内存大小，单位Byte
@Param   :	szFile	--	申请内存的源文件地址
@Param   :	nLine	--	申请内存的源文件行号
@Return  :	成功返回申请后内存地址，失败返回NULL
@Modify  :
@Remark  :	
 *************************************************************************/
void* MyMem_Malloc(IN unsigned long ulSize, IN const char* szFile, IN int nLine)
{
	void* pAddr = NULL;

	if (ulSize == 0)
	{
		return NULL;
	}

	pAddr = malloc(ulSize);
	if (pAddr == NULL)
	{
		LOG_E("Mem_Malloc malloc err[%#x]", CF_GetLastError());
		return NULL;
	}

	if (g_nMemDbgStatus == MEM_STATUS_ON)
	{
		MyMemDbg_Malloc(pAddr, ulSize, szFile, nLine);
	}

	return pAddr;
}


/*************************************************************************
@Purpose :	释放申请的内存
@Param   :	addr	--	由Mem_Malloc返回的内存地址
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMem_Free(IN void* addr)
{
	if (addr == NULL)
	{
		return ;
	}

	if (g_nMemDbgStatus == MEM_STATUS_ON)
	{
		MyMemDbg_Free(addr);
	}

	free(addr);
	addr = NULL;
}

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
void* MyMem_Realloc(IN void* addr, IN unsigned long ulSize, IN const char* szFile, IN int nLine)
{
	void* pAddr = NULL;

	pAddr = realloc(addr, ulSize);
	if (pAddr == NULL)
	{
		LOG_E("realloc malloc err[%#x]", CF_GetLastError());
		return NULL;
	}

	if (g_nMemDbgStatus == MEM_STATUS_ON)
	{
		MyMemDbg_Realloc(addr, pAddr, ulSize, szFile, nLine);
	}

	return pAddr;
}

/*************************************************************************
@Purpose :	打印内存泄露信息
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMem_Leak(void)
{
	MyMemDbg_Leak();
}