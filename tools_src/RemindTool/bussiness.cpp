#include "stdafx.h"

#define BUSS_INFO_FILE	"info\\buss_info.bin"



int LoadBussInfoList(char* szPath, vector<BUSS_INFO>* list)
{
	FILE*		fp;
	BUSS_INFO	bi;
	int			nLen;
	char		szFile[MAX_PATH];

	BUFCLR(szFile);
	sprintf(szFile, "%s\\%s", szPath, BUSS_INFO_FILE);
	fp = fopen(szFile, "rb");
	if (fp == NULL)
	{
		LOG_E("Open file [%s] error [%#x]", szFile, GetLastError());
		return ERR_FILE_OPEN;
	}

	while (!feof(fp))
	{
		memset(&bi, 0, sizeof(bi));
		nLen = fread(&bi, sizeof(BUSS_INFO), 1, fp);
		if (nLen == 0)
		{
			break;
		}
		list->push_back(bi);
	}

	fclose(fp);

	return ERR_OK;
}


int SaveBussInfoFile(char* szPath, vector<BUSS_INFO>* list)
{
	FILE*		fp;
	vector<BUSS_INFO>::iterator		it;
	char		szFile[MAX_PATH];

	BUFCLR(szFile);
	sprintf(szFile, "%s\\%s", szPath, BUSS_INFO_FILE);

	fp = fopen(szFile, "wb");
	if (fp == NULL)
	{
		LOG_E("Open file [%s] error [%#x]", szFile, GetLastError());
		return ERR_FILE_OPEN;
	}

	for (it = list->begin(); it != list->end(); it++)
	{
		fwrite(&*it, sizeof(BUSS_INFO), 1, fp);
	}

	fclose(fp);

	return ERR_OK;
}

