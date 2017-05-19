#ifndef __MEM_H__
#define __MEM_H__

#ifdef  __cplusplus
extern "C" {
#endif

#define _MEM								//����ú����ʵ��

#define MEM_STATUS_ON		1				//�ڴ���Դ�
#define MEM_STATUS_OFF		0				//�ڴ���Թر�

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
@Purpose :	�����ڴ����״̬
@Param   :
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
void MyMem_SetDbg(int nState);

/*************************************************************************
@Purpose :	���ڴ����
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :Ĭ��״̬��Ϊ�ر��ڴ����״̬
 *************************************************************************/
#define Mem_SetDbgOn()		MyMem_SetDbg(MEM_STATUS_ON)

/*************************************************************************
@Purpose :	�ر��ڴ����
@Param   :	None
@Return  :	None
@Modify  :
@Remark  :
 *************************************************************************/
#define Mem_SetDbgOff()		MyMem_SetDbg(MEM_STATUS_OFF)

/*************************************************************************
@Purpose :	�����ڴ�
@Param   :	ulSize	--	�����ڴ��С����λByte
@Param   :	szFile	--	�����ڴ��Դ�ļ���ַ
@Param   :	nLine	--	�����ڴ��Դ�ļ��к�
@Return  :	�ɹ�����������ڴ��ַ��ʧ�ܷ���NULL
@Modify  :
@Remark  :
 *************************************************************************/
void* MyMem_Malloc(IN unsigned long ulSize, IN const char* szFile, IN int nLine);

/*************************************************************************
@Purpose :	�ͷ�������ڴ�
@Param   :	addr	--	��Mem_Malloc���ص��ڴ��ַ
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMem_Free(IN void* addr);

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
void* MyMem_Realloc(IN void* addr, IN unsigned long ulSize, IN const char* szFile, IN int nLine);

/*************************************************************************
@Purpose :	��ӡ�ڴ�й¶��Ϣ
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

