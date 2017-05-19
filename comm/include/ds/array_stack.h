/*********************************************************************
*:
*:
*:          Author: dspeeding
*:          Copyright (c) 2012, dspeeding
*:
*:        Created at: 2012.06.15
*:     Last modified: 2016.03.08
*:
*:      Introduction: the array stack
*:
*:
*:*********************************************************************/
#ifndef _ARRAYSTACK_H_
#define _ARRAYSTACK_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct TDefArrayStack
{
	int		nTop;
	int		nDataSize;
	int		nMaxNum;
	void*	pArray;
}ArrayStack;

typedef ArrayStack* PArrayStack;
typedef const ArrayStack CArrayStack;
typedef const ArrayStack* PCArrayStack;

/*************************************************************************
@Purpose :	init stack
@Param   :	ppStack     -- stack pointer
@Param   :	nElemSize	-- element data size
@Param   :	nMaxNum		-- max num of stack
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int InitArrayStack(IN PArrayStack* ppStack, IN int nElemSize, IN int nMaxNum);

/*************************************************************************
@Purpose :	judge is array stack is empty
@Param   :	pStack      -- stack pointer
@Return  :	0 -- is empty ,-1 -- not empty
@Modify  :
@Remark  :
*************************************************************************/
int IsEmptyArrayStack(IN PArrayStack pStack);

/*************************************************************************
@Purpose :	judge is array stack is full
@Param   :	pStack      -- stack pointer
@Return  :	0 -- is full ,-1 -- not full
@Modify  :
@Remark  :
*************************************************************************/
int IsFullArrayStack(IN PArrayStack pStack);

/*************************************************************************
@Purpose :	push data to stack
@Param   :	pStack      -- stack pointer
@Param   :	pVal		-- element which to push
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int PushArrayStack(IN PArrayStack pStack, IN void* pVal);

/*************************************************************************
@Purpose :	pop data from stack
@Param   :	pStack      -- stack pointer
@Param   :	pVal		-- element buffer
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int PopArrayStack(IN PArrayStack pStack, OUT void* pVal);

/*************************************************************************
@Purpose :	get the top data from stack
@Param   :	pStack      -- stack pointer
@Param   :	pVal		-- element buffer
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int GetTopArrayStack(IN PArrayStack pStack, OUT void* pVal);

/*************************************************************************
@Purpose :	destroy stack,free memory
@Param   :	pStack      -- stack pointer
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void DestroyArrayStack(IN PArrayStack pStack);

#ifdef __cplusplus
}
#endif

#endif
