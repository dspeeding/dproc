#include "basetype.h"
#include "Mem.h"
#include "MemDbg.h"

//��ǰ״̬Ϊδ����dbg�Ⱥ���
int		g_nMemDbgStatus = MEM_STATUS_OFF;		

/*************************************************************************
@Purpose :	�����ڴ����״̬
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
@Purpose :	�����ڴ�
@Param   :	ulSize	--	�����ڴ��С����λByte
@Param   :	szFile	--	�����ڴ��Դ�ļ���ַ
@Param   :	nLine	--	�����ڴ��Դ�ļ��к�
@Return  :	�ɹ�����������ڴ��ַ��ʧ�ܷ���NULL
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
@Purpose :	�ͷ�������ڴ�
@Param   :	addr	--	��Mem_Malloc���ص��ڴ��ַ
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
@Purpose :	���������ڴ�
@Param   :	addr	--	ԭ�ڴ��ַ
@Param   :	dwSize	--	�����ڴ��С����λByte
@Param   :	szFile	--	�����ڴ��Դ�ļ���ַ
@Param   :	nLine	--	�����ڴ��Դ�ļ��к�
@Return  :	�ɹ�������������ڴ��ַ��ʧ�ܷ���NULL
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
@Purpose :	��ӡ�ڴ�й¶��Ϣ
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMem_Leak(void)
{
	MyMemDbg_Leak();
}