#include "basetype.h"

#pragma comment(lib, "dproclib.lib")


void TestHash()
{
	int			nRet;
	LINK_HASH*	pHash;

	char*		data;
	char		temp[128];
	char*		old_data;

	nRet = LH_Init(&pHash, NULL, NULL);
	if (nRet != ERR_OK)
	{
		LOG_E("LH_Init error");
		return;
	}

	for (int i = 0; i < 100; i++)
	{
		data = (char*)Mem_Malloc(128);
		sprintf(data, "abc_%03d", i);

		nRet = LH_Insert(pHash, data, &old_data);
		if (nRet != ERR_OK)
		{
			LOG_E("LH_Insert error");
			goto err;
		}

		if (old_data != NULL)
		{
			Mem_Free(old_data);
		}
		system("cls");
		
		LH_Node_Stats(pHash, stdout);
		Sleep(500);
		
	}
	
	LH_Stats(pHash, stdout);
	LH_Node_Usage_Stats(pHash, stdout);

	for (int i = 0; i < 100; i++)
	{
		sprintf(temp, "abc_%03d", i);

		nRet = LH_Delete(pHash, temp, &data);
		if (nRet != ERR_OK)
		{
			LOG_E("LH_Delete error");
			goto err;
		}
		Mem_Free(data);
		LH_Node_Stats(pHash, stdout);
		system("cls");
	}

err:
	LH_Destory(pHash);
}


int main(int argc, char* argv[])
{
	Mem_SetDbgOn();
	LOG_Init(LOG_LEV_DEBUG, "TestHash", "TestHash");
	TestHash();
	Mem_Leak();
	return 0;
}
