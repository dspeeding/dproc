#pragma once


#define MAX_TASK_PROC_NUM		11
#define GET_HTML_DATA_TIME_OUT	10

/*************************************************************************
 Purpose :	�����̴߳���
 Input   :  parameter	--	��UI��ָ��
 Return  :	None
 Remark  :	
 *************************************************************************/
void WorkThreadProc(void* parameter);

/*************************************************************************
Purpose ��	�����̴߳���
Input   :  parameter	--	�����߳̽ṹָ��
Return  :	None
Remark  :
*************************************************************************/
void CheckThreadProc(void* parameter);

