#pragma once


#define MAX_TASK_PROC_NUM		11
#define GET_HTML_DATA_TIME_OUT	10

/*************************************************************************
 Purpose :	工作线程处理
 Input   :  parameter	--	主UI类指针
 Return  :	None
 Remark  :	
 *************************************************************************/
void WorkThreadProc(void* parameter);

/*************************************************************************
Purpose ：	工作线程处理
Input   :  parameter	--	工作线程结构指针
Return  :	None
Remark  :
*************************************************************************/
void CheckThreadProc(void* parameter);

