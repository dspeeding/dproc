#include "basetype.h"


void Test_3()
{
	int a = 1;
	int b = 0;

	b = a != 0 ? 3 : 4;
	LOG_D("b=[%d]", b);
}


int Test_func(int n)
{
	if (n < 2) {
		return n;
	}
	return Test_func(n - 2) + Test_func(n - 1);
}

int Test_func2(int n)
{
	int i = 0;
	int a = 1;
	int b = 1;
	if (n <= 2)
	{
		return n;
	}
	for (i = 2; i <= (n + 1) / 2; i++)
	{
		a = a + b;
		b = a + b;
	}
	if (n % 2 == 0)
	{
		return b;
	}
	else
	{
		return a;
	}

}

int Test_sscanf()
{
	int ct = 0;
	char szTemp[512] = { 0 };
	char szDes[5][128];

	memset(szDes, 0, 5*128);

	sprintf(szTemp, "*/3 * * * *");
	ct = sscanf(szTemp, "%s%s%s%s%s", szDes[0], szDes[1], szDes[2], szDes[3], szDes[4]);
	LOG_D("ct=[%d] [%s][%s][%s][%s][%s]", ct, szDes[0], szDes[1], szDes[2], szDes[3], szDes[4]);


	sprintf(szTemp, "*/4   234   abv e4  a3 agf#");
	ct = sscanf(szTemp, "%s%s%s%s%s", szDes[0], szDes[1], szDes[2], szDes[3], szDes[4]);
	LOG_D("ct=[%d] [%s][%s][%s][%s][%s]", ct, szDes[0], szDes[1], szDes[2], szDes[3], szDes[4]);

	sprintf(szTemp, "*/4   234  ");
	ct = sscanf(szTemp, "%s%s%s%s%s", szDes[0], szDes[1], szDes[2], szDes[3], szDes[4]);
	LOG_D("ct=[%d] [%s][%s][%s][%s][%s]", ct, szDes[0], szDes[1], szDes[2], szDes[3], szDes[4]);

	return 0;
}


#define SET_BIT(a,n) (a[(n)/8] |= (1<<(7-(n)%8)))
int Test_bits()
{
	int i;
	unsigned char mask[(60 + 7) / 8] = { 0 };
	char	temp[128] = { 0 };

	for (i = 0; i < 60; i++)
	{
		SET_BIT(mask, i);
		STR_BIN2HEX(mask, sizeof(mask), sizeof(temp), temp);
		LOG_D("mask is [%s]", temp);
		memset(mask, 0, sizeof(mask));
	}

	return 0;
}

void Test_WriteFile()
{
	int		i;
	FILE*	fp;

	LOG_FUNC_START();

	fp = fopen("abc.txt", "w");
	if (fp == NULL)
	{
		LOG_E("open file error[%#x]", CF_GetLastError());
		return;
	}

	for (i = 0; i < 1000000; i++)
	{
		fprintf(fp, "1234567890abcdefg\n");
	}

	fclose(fp);
	LOG_FUNC_END();
}

static int mon_log(const char* format, ...)
{
	char	a[20];
	char str_tmp[10];
	char	b[20];
	int i = 0;
	va_list vArgList;

	memset(a, 'a', sizeof(a));
	a[19] = 0;
	memset(b, 'b', sizeof(b));
	b[19] = 0;

	LOG_D("a=[%s]", a);
	LOG_D("b=[%s]", b);

	va_start(vArgList, format);
	i = vsnprintf(str_tmp, 10, format, vArgList);
	va_end(vArgList);

	//str_tmp[11] = 'c';

	LOG_D("a=[%s]", a);
	LOG_D("b=[%s]", b);
	LOG_D("%s", str_tmp);
	return i;
}

void Test_vsnprintf()
{
	int i = mon_log("%s,%d,%d,%d", "abcdef", 2, 3, 4); 
	LOG_D("%d", i);
}

void Test_malloc()
{
	int				i = 0;
	unsigned char*	p[10];

	for (i = 0; i < 10; i++)
	{
		p[i] = (unsigned char*)Mem_Malloc(100);
		//LOG_D("[%d] --p[%p]", i, p[i]);
		
	}
	for (i = 0; i < 10; i++)
	{
		Mem_Free(p[i]);
	}
}

void Test_Other()
{
// 	Test_3();
// 
// 	LOG_D("Test_func start...");
// 	int n = Test_func(40);
// 	LOG_D("Test_func end...[%d]", n);
// 
// 	LOG_D("Test_func2 start...");
// 	n = Test_func2(40);
// 	LOG_D("Test_func2 end...[%d]", n);
// 
// 	Test_sscanf();
// 
// 	Test_bits();
// 
// 	Test_WriteFile();
// 	Test_vsnprintf();

	Test_malloc();
}