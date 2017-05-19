#include "basetype.h"
#include "ds/array_queue.h"

/*************************************************************************
@Purpose :	初始化循环队列
@Param   :	ppQueue     -- queue pointer
@Param   :	nElemSize	-- element data size
@Param   :	nMaxNum		-- max num of queue
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InitArrayQueue(OUT PArrayQueue* ppQueue, IN int nElemSize, IN int nMaxNum)
{
	PArrayQueue		pQueue;

	CHECK_POINTER(ppQueue, ERR_COMM_NULL_POINTER);

	if ((nMaxNum <= 0) || (nElemSize <= 0))
	{
		LOG_E("param error ,nElemSize=[%d] nMaxNum=[%d]", nElemSize, nMaxNum);
		return ERR_COMM_PARAM;
	}

	*ppQueue = (PArrayQueue)Mem_Malloc(sizeof(ArrayQueue));
	if (*ppQueue == NULL)
	{
		LOG_E("malloc *ppQueue error,error=[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	pQueue = *ppQueue;
	memset(pQueue, 0, sizeof(ArrayQueue));
			
	pQueue->pArray = (void*)Mem_Malloc(nMaxNum * nElemSize);
	if (pQueue->pArray == NULL)
	{
		LOG_E("malloc pQueue->pArray error,error=[%#x]", CF_GetLastError());
		Mem_Free(pQueue);
		*ppQueue = NULL;
		return ERR_COMM_MEM_MALLOC;
	}

	memset(pQueue->pArray, 0, nMaxNum * nElemSize);

	pQueue->nDataSize = nElemSize;
	pQueue->nFront = 0;
	pQueue->nRear = -1;
	pQueue->nLen = 0;
	pQueue->nMaxLen = nMaxNum;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	初始化循环队列
@Param   :	ppQueue     -- queue pointer
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int AgainMallocArrayQueue(PArrayQueue pQueue)
{
	int		i;
	void*	p;

	p = Mem_Realloc(pQueue->pArray, 2 * pQueue->nMaxLen * pQueue->nDataSize);
	if (p == NULL)
	{
		LOG_E("realloc p error,err=[%#x]", CF_GetLastError());
		return ERR_COMM_MEM_MALLOC;
	}

	memset((unsigned char*)p + pQueue->nMaxLen * pQueue->nDataSize, 0, pQueue->nMaxLen * pQueue->nDataSize);

	pQueue->pArray = p;
	//把原队列的尾部内容后移maxLen个位置
	if (pQueue->nRear != pQueue->nMaxLen - 1)
	{
		for (i = 0; i <= pQueue->nRear; i++)
		{
			memcpy((unsigned char*)pQueue->pArray + (i + pQueue->nMaxLen) * pQueue->nDataSize,
				(unsigned char*)pQueue->pArray + i * pQueue->nDataSize, pQueue->nDataSize);
			memset((unsigned char*)pQueue->pArray + i * pQueue->nDataSize, 0, pQueue->nDataSize);
		}
		//队尾指针后移maxLen 个位置
		pQueue->nRear += pQueue->nMaxLen;
	}
	pQueue->nMaxLen = 2 * pQueue->nMaxLen;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	向队列中插入元素
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	AddArrayQueue(IN PArrayQueue pQueue, IN void* pData)
{
	int		nRet;

	CHECK_POINTER(pQueue, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pData, ERR_COMM_NULL_POINTER);

	if (IsFullArrayQueue(pQueue) == 1)
	{
		nRet = AgainMallocArrayQueue(pQueue);
		if (nRet != ERR_OK)
		{
			LOG_E("AgainMallocArrayQueue error[%#x]", nRet);
			return nRet;
		}
	}

	//求出队尾的下一个位置
	pQueue->nRear = (pQueue->nRear + 1) % pQueue->nMaxLen;
	memcpy((unsigned char*)pQueue->pArray + pQueue->nDataSize * pQueue->nRear, pData, pQueue->nDataSize);
	pQueue->nLen++;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	从队列中删除元素
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	DelArrayQueue(IN PArrayQueue pQueue, OUT void* pData)
{
	CHECK_POINTER(pQueue, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pData, ERR_COMM_NULL_POINTER);
	
	if (IsEmptyArrayQueue(pQueue) == 1)
	{
		LOG_E("queue is empty now");
		return ERR_COMM_OUT_OF_INDEX;
	}

	memcpy(pData, (unsigned char*)pQueue->pArray + pQueue->nDataSize * pQueue->nFront, pQueue->nDataSize);
	memset((unsigned char*)pQueue->pArray + pQueue->nDataSize * pQueue->nFront, 0, pQueue->nDataSize);

	//队首指针指向下一个位置
	pQueue->nFront = (pQueue->nFront + 1) % pQueue->nMaxLen;
	pQueue->nLen--;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	从队列中提取元素
@Param   :	pQueue		-- queue pointer
@Param   :	pData		-- data
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int	PeekArrayQueue(IN PArrayQueue pQueue, OUT void* pData)
{
	CHECK_POINTER(pQueue, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(pData, ERR_COMM_NULL_POINTER);

	if (IsEmptyArrayQueue(pQueue) == 1)
	{
		LOG_E("queue is empty now");
		return ERR_COMM_OUT_OF_INDEX;
	}

	memcpy(pData, (unsigned char*)pQueue->pArray + pQueue->nDataSize * pQueue->nFront, pQueue->nDataSize);

	return ERR_OK;
}

/*************************************************************************
@Purpose :	判断队列是否为空
@Param   :	pQueue		-- queue pointer
@Return  :	1 -- 空 , 0 -- 非空
@Modify  :	
@Remark  :	
*************************************************************************/
int	IsEmptyArrayQueue(IN PArrayQueue pQueue)
{
	if (pQueue && (pQueue->nLen == 0))
	{
		return 1;
	}

	return 0;
}

/*************************************************************************
@Purpose :	判断队列是否已满
@Param   :	pQueue		-- queue pointer
@Return  :	1 -- 满 , 0 -- 未满
@Modify  :
@Remark  :
*************************************************************************/
int	IsFullArrayQueue(IN PArrayQueue pQueue)
{
	if (pQueue && (pQueue->nLen == pQueue->nMaxLen))
	{
		return 1;
	}

	return 0;
}

/*************************************************************************
@Purpose :	获取队列长度
@Param   :	pQueue		-- queue pointer
@Return  :	队列元素个数
@Modify  :
@Remark  :
*************************************************************************/
int	GetArrayQueueLen(IN PArrayQueue pQueue)
{
	if (pQueue)
	{
		return pQueue->nLen;
	}
	return 0;
}


/*************************************************************************
@Purpose :	清空队列
@Param   :	pQueue		-- queue pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void ClearArrayQueue(IN PArrayQueue pQueue)
{
	if (pQueue == NULL)
	{
		return;
	}

	if (pQueue->pArray != NULL)
	{
		memset(pQueue->pArray, 0, pQueue->nDataSize * pQueue->nMaxLen);
	}

	pQueue->nFront = 0;
	pQueue->nRear = -1;
	pQueue->nLen = 0;

}

/*************************************************************************
@Purpose :	销毁队列
@Param   :	pQueue		-- queue pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyArrayQueue(IN PArrayQueue pQueue)
{
	if (pQueue == NULL)
	{
		return;
	}

	if (pQueue->pArray != NULL)
	{
		Mem_Free(pQueue->pArray);
	}
	Mem_Free(pQueue);
	pQueue = NULL;
}