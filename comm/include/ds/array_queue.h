#pragma once
//循环队列的数组实现



typedef struct ArrayQueue_ST
{
	int			nDataSize;		//数据元素的大小
	int			nFront;			//队首指针
	int			nRear;			//队尾指针
	int			nLen;			//队列当前个数
	int			nMaxLen;		//数组最大个数
	void*		pArray;			//指向队列的数组空间的指针
}ArrayQueue;

typedef ArrayQueue* PArrayQueue;

#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************
@Purpose :	初始化循环队列
@Param   :	ppQueue     -- queue pointer
@Param   :	nElemSize	-- element data size
@Param   :	nMaxNum		-- max num of queue
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InitArrayQueue(OUT PArrayQueue* ppQueue, IN int nElemSize, IN int nMaxNum);

/*************************************************************************
@Purpose :	向队列中插入元素
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AddArrayQueue(IN PArrayQueue pQueue, IN void* pData);

/*************************************************************************
@Purpose :	从队列中删除元素
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	DelArrayQueue(IN PArrayQueue pQueue, OUT void* pData);

/*************************************************************************
@Purpose :	从队列中提取元素
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	PeekArrayQueue(IN PArrayQueue pQueue, OUT void* pData);

/*************************************************************************
@Purpose :	判断队列是否为空
@Param   :	pQueue		-- queue pointer
@Return  :	1 -- 空 , 0 -- 非空
@Modify  :
@Remark  :
*************************************************************************/
int	IsEmptyArrayQueue(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	判断队列是否已满
@Param   :	pQueue		-- queue pointer
@Return  :	1 -- 满 , 0 -- 未满
@Modify  :
@Remark  :
*************************************************************************/
int	IsFullArrayQueue(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	获取队列长度
@Param   :	pQueue		-- queue pointer
@Return  :	队列元素个数
@Modify  :
@Remark  :
*************************************************************************/
int	GetArrayQueueLen(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	清空队列
@Param   :	pQueue		-- queue pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void ClearArrayQueue(IN PArrayQueue pQueue);

/*************************************************************************
@Purpose :	销毁队列
@Param   :	pQueue		-- queue pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyArrayQueue(IN PArrayQueue pQueue);

#ifdef __cplusplus
}
#endif


