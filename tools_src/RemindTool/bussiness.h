#pragma once
using namespace std;
#include <vector>

#define	MAX_CHECK_TIME_NUM		15			//需要检查的时间

typedef struct _TDEF_BUSS_INFO
{
	char		szComp[64];
	char		szName[32];
	char		szTel[32];
	char		szTips[MAX_CHECK_TIME_NUM][255];
	char		szDeadline[MAX_CHECK_TIME_NUM][10];
}BUSS_INFO;

typedef BUSS_INFO*	PBUSS_INFO;


int LoadBussInfoList(char* szPath, vector<BUSS_INFO>* list);
int SaveBussInfoFile(char* szPath, vector<BUSS_INFO>* list);


