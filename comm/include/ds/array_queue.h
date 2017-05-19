#pragma once
//ѭ�����е�����ʵ��



typedef struct ArrayQueue_ST
{
	int			nDataSize;		//����Ԫ�صĴ�С
	int			nFront;			//����ָ��
	int			nRear;			//��βָ��
	int			nLen;			//���е�ǰ����
	int			nMaxLen;		//����������
	void*		pArray;			//ָ����е�����ռ��ָ��
}ArrayQueue;

typedef ArrayQueue* PArrayQueue;

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************
@Purpose :	��ʼ��ѭ������
@Param   :	ppQueue     -- queue pointer
@Param   :	nElemSize	-- element data size
@Param   :	nMaxNum		-- max num of queue
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InitArrayQueue(OUT PArrayQueue* ppQueue, IN int nElemSize, IN int nMaxNum);

/*************************************************************************
@Purpose :	������в���Ԫ��
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AddArrayQueue(IN PArrayQueue pQueue, IN void* pData);

/*************************************************************************
@Purpose :	�Ӷ�����ɾ��Ԫ��
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	DelArrayQueue(IN PArrayQueue pQueue, OUT void* pData);

/*************************************************************************
@Purpose :	�Ӷ�������ȡԪ��
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	PeekArrayQueue(IN PArrayQueue pQueue, OUT void* pData);

/*************************************************************************
@Purpose :	�ж϶����Ƿ�Ϊ��
@Param   :	pQueue		-- queue pointer
@Return  :	1 -- �� , 0 -- �ǿ�
@Modify  :
@Remark  :
*************************************************************************/
int	IsEmptyArrayQueue(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	�ж϶����Ƿ�����
@Param   :	pQueue		-- queue pointer
@Return  :	1 -- �� , 0 -- δ��
@Modify  :
@Remark  :
*************************************************************************/
int	IsFullArrayQueue(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	��ȡ���г���
@Param   :	pQueue		-- queue pointer
@Return  :	����Ԫ�ظ���
@Modify  :
@Remark  :
*************************************************************************/
int	GetArrayQueueLen(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	��ն���
@Param   :	pQueue		-- queue pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void ClearArrayQueue(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	���ٶ���
@Param   :	pQueue		-- queue pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyArrayQueue(IN PArrayQueue pQueue);

#ifdef __cplusplus
}
#endif


