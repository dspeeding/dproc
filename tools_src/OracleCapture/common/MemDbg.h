#ifndef __MEM_DBG_H__
#define __MEM_DBG_H__

#include "basetype.h"

#ifdef  __cplusplus
extern "C" {
#endif

//�����ڴ��¼�ṹ��
typedef struct mem_st
{
    void*			addr;					//�����ڴ�ĵ�ַ
    unsigned long	num;					//�����ڴ�Ĵ�С
    char 			file[MAX_FILE_PATH];	//�����ڴ浱ʱ��Դ�����ļ�
    int				line;					//�����ڴ浱ʱ��Դ�����к�
    unsigned long	threadid;				//�����ڴ���߳�ID
    unsigned long	order;					//�ڼ����ڴ����
    time_t			time;					//�����ڴ�ʱ��
} MEM;

typedef MEM* PMEM;
typedef const MEM CMEM;
typedef const MEM* PCMEM;


//��¼�ڴ�й¶�����Ϣ
typedef struct mem_leak_st 
{
    int		chunks;				//�����˶��ٴ��ڴ�й¶
    long	bytes;				//һ���ж����ֽ��ڴ汻й¶
} MEM_LEAK;


/*************************************************************************
@Purpose :	�����ڴ����
@Param   :	addr	--	���뵽�ĵ�ַ
@Param   : ulSize	--	�����ڴ��С����λByte
@Param   : szFile	--	�����ڴ��Դ�ļ���ַ
@Param   : nLine	--	�����ڴ��Դ�ļ��к�
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMemDbg_Malloc(void* addr, unsigned long ulSize, const char* szFile, int nLine);

/*************************************************************************
@Purpose :	�ͷ��ڴ����
@Param   :	addr	-- �ڴ��ַ
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMemDbg_Free(void* addr);

/*************************************************************************
@Purpose :	���������ڴ�
@Param   :	addrOld	--	[IN] ԭ�ڴ��ַ
@Param   :	addrNew	--	[IN] ���ڴ��ַ
@Param   :	ulSize	--	[IN] �����ڴ��С����λByte
@Param   :	szFile	--	[IN] �����ڴ��Դ�ļ���ַ
@Param   :	nLine	--	[IN] �����ڴ��Դ�ļ��к�
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void MyMemDbg_Realloc(void* addrOld, void* addrNew, unsigned long ulSize, const char* szFile, int nLine);

/*************************************************************************
@Purpose :	��ӡ�ڴ�й¶��Ϣ
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


