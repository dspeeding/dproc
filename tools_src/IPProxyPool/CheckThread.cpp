#include "stdafx.h"
#include "IPProxyPool.h"
#include "IPProxyPoolDlg.h"
#include "curl_comm.h"

void CheckProc(CHECKINFO* pWorkInfo, char* buffer)
{
	int			ret;
	PROXY_INFO	pi;
	char		ipport[128] = { 0 };
	char		url[128] = { 0 };
	int			nCt = 0;

	memset(&pi, 0, sizeof(pi));
	pWorkInfo->pDlg->LockData();
	ret = DelArrayQueue(pWorkInfo->pDlg->m_Queue, &pi);
	pWorkInfo->pDlg->UnLockData();
	ReleaseSemaphore(pWorkInfo->pDlg->m_hFullSemp, 1, NULL);

	if (ret != ERR_OK)
	{
		LOG_E("get data error");
		return;
	}

// 	LOG_D("thrd[%d] get data  ip[%s] port[%s] area[%s] type[%s] protocol[%s] delay[%s] time[%s]",
// 		pWorkInfo->nWorkID, pi.ip, pi.port, pi.area, pi.type, pi.protocol, pi.delay, pi.time);

	sprintf(ipport, "%s:%s", pi.ip, pi.port);
	sprintf(url, "http://httpbin.org/ip");

	

	ret = get_html_data(url, buffer, GET_HTML_DATA_TIME_OUT, NULL, ipport);
	if (ret)
	{
		WaitForSingleObject(pWorkInfo->pDlg->m_hMutex_UI, INFINITE);
		pWorkInfo->pDlg->m_fail_num++;
		pWorkInfo->pDlg->m_task_statis[pi.nTaskID].nFailNum++;
		ReleaseMutex(pWorkInfo->pDlg->m_hMutex_UI);
//		LOG_E("check ip[%s] error", ipport);
		return;
	}

 	LOG_D("thrd[%d] ip[%s] port[%s] area[%s] type[%s] protocol[%s] delay[%s] time[%s]  Success",
 		pWorkInfo->nWorkID, pi.ip, pi.port, pi.area, pi.type, pi.protocol, pi.delay, pi.time);

	WaitForSingleObject(pWorkInfo->pDlg->m_hMutex_UI, INFINITE);
	nCt = pWorkInfo->pDlg->m_IPList.GetItemCount();
	pWorkInfo->pDlg->m_IPList.InsertItem(nCt, pi.ip);
	pWorkInfo->pDlg->m_task_statis[pi.nTaskID].nSuccessNum++;
	ReleaseMutex(pWorkInfo->pDlg->m_hMutex_UI);

	pWorkInfo->pDlg->m_IPList.SetItemText(nCt, 1, pi.port);
	pWorkInfo->pDlg->m_IPList.SetItemText(nCt, 2, pi.type);
	pWorkInfo->pDlg->m_IPList.SetItemText(nCt, 3, pi.protocol);
	pWorkInfo->pDlg->m_IPList.SetItemText(nCt, 4, pi.delay);
	pWorkInfo->pDlg->m_IPList.SetItemText(nCt, 5, pi.time);
	pWorkInfo->pDlg->m_IPList.SetItemText(nCt, 6, pi.area);
	pWorkInfo->pDlg->m_IPList.EnsureVisible(nCt, FALSE);
}

/*************************************************************************
Purpose ：	工作线程处理
Input   :  parameter	--	工作线程结构指针
Return  :	None
Remark  :
*************************************************************************/
void CheckThreadProc(void* parameter)
{
	DWORD		dwRet = 0;
	CHECKINFO*	pWorkInfo = (CHECKINFO*)parameter;

	LOG_D("分析线程[%02d]进入主循环...", pWorkInfo->nWorkID);
	while (pWorkInfo->nWorkCircle)
	{
		//首先更新线程时间
		pWorkInfo->lTime = GetTickCount();
		//等待work事件
		dwRet = WaitForSingleObject(pWorkInfo->pDlg->m_hSemp, 500);
		switch (dwRet)
		{
		case WAIT_TIMEOUT:
			break;
		case WAIT_OBJECT_0:
//			LOG_D("[%d]接收到工作消息，开始工作", pWorkInfo->nWorkID);
			pWorkInfo->nIsWorking = 1;
			memset(pWorkInfo->pBuffer, 0, MAX_BUFFER_SIZE);
			CheckProc(pWorkInfo, pWorkInfo->pBuffer);
			pWorkInfo->nIsWorking = 0;
			break;
		}
	}

	LOG_D("分析线程[%02d]退出主循环...", pWorkInfo->nWorkID);
}

