#include <stdio.h>

#include "basetype.h"
#include "test_sqlist.h"
#include "test_other.h"
#include "gtest/gtest.h"

#define LOG_PRIFX			"TestComm"
#define LOG_FILE			"TestComm"

TEST(TestComm, T_Log)
{
	unsigned char bTemp[128] = { 0 };

	LOG_FUNC_START();

	LOG_D("test1");
	LOG_D("hello %s", "world");

	LOG_I("test1");
	LOG_I("hello %s", "world");

	LOG_W("test1");
	LOG_W("hello %s", "world");

	LOG_E("test1");
	LOG_E("hello %s", "world");

	sprintf((char*)bTemp, "123456");
	LOGBIN_D(bTemp, 6);
	LOGBIN_I(bTemp, 6);
	LOGBIN_W(bTemp, 6);
	LOGBIN_E(bTemp, 6);
	LOGBIN_D(NULL, 6);

	LOG_FUNC_END();
}

TEST(TestComm, T_Trim)
{
	char	szTest[128];

	LOG_FUNC_START();

	sprintf(szTest, "    test hello world    ");
	LOG_D("src = [%s]", szTest);

	sprintf(szTest, "    test hello world    ");
	EXPECT_STREQ("test hello world    ", STR_Trim(szTest, TRIM_LEFT));
	LOG_D("TrimLeft = [%s]", szTest);

	sprintf(szTest, "    test hello world    ");
	EXPECT_STREQ("    test hello world", STR_Trim(szTest, TRIM_RIGHT));
	LOG_D("TrimRight = [%s]", szTest);

	sprintf(szTest, "    test hello world    ");
	EXPECT_STREQ("test hello world", STR_Trim(szTest, TRIM_LEFT | TRIM_RIGHT));
	LOG_D("TrimLeftRight = [%s]", szTest);

	sprintf(szTest, "    test hello world    ");
	EXPECT_STREQ("testhelloworld", STR_Trim(szTest, TRIM_ALL));
	LOG_D("TrimALL = [%s]", szTest);

	sprintf(szTest, "       ");
	EXPECT_STREQ("", STR_Trim(szTest, TRIM_LEFT));
	LOG_D("TrimLeft = [%s]", szTest);

	sprintf(szTest, "       ");
	EXPECT_STREQ("", STR_Trim(szTest, TRIM_RIGHT));
	LOG_D("TrimRight = [%s]", szTest);

	sprintf(szTest, "       ");
	EXPECT_STREQ("", STR_Trim(szTest, TRIM_LEFT | TRIM_RIGHT));
	LOG_D("TrimLeftRight = [%s]", szTest);

	sprintf(szTest, "       ");
	EXPECT_STREQ("", STR_Trim(szTest, TRIM_ALL));
	LOG_D("TrimALL = [%s]", szTest);

	sprintf(szTest, "abcdefg12345");
	EXPECT_STREQ("abcdefg12345", STR_Trim(szTest, TRIM_LEFT));
	LOG_D("TrimLeft = [%s]", szTest);

	sprintf(szTest, "abcdefg12345");
	EXPECT_STREQ("abcdefg12345", STR_Trim(szTest, TRIM_RIGHT));
	LOG_D("TrimRight = [%s]", szTest);

	sprintf(szTest, "abcdefg12345");
	EXPECT_STREQ("abcdefg12345", STR_Trim(szTest, TRIM_LEFT | TRIM_RIGHT));
	LOG_D("TrimLeftRight = [%s]", szTest);

	sprintf(szTest, "abcdefg12345");
	EXPECT_STREQ("abcdefg12345", STR_Trim(szTest, TRIM_ALL));
	LOG_D("TrimALL = [%s]", szTest);

	LOG_FUNC_END();
}

TEST(TestComm, T_Base64)
{
	char	szSrc[128];
	char	szBase64Enc[256];
	char	szBase64Dec[256];
	int		nDecLen = 0;

	LOG_FUNC_START();
	BUFCLR(szSrc);
	BUFCLR(szBase64Enc);
	BUFCLR(szBase64Dec);

	strcpy(szSrc, "12345678");
	LOG_D("src[%s]", szSrc);
	Base64_Encode(szSrc, strlen(szSrc), szBase64Enc, sizeof(szBase64Enc));
	EXPECT_STRCASEEQ("MTIzNDU2Nzg=", szBase64Enc);

	LOG_D("base64[%s]", szBase64Enc);
	nDecLen = Base64_Decode(szBase64Enc, strlen(szBase64Enc), (unsigned char*)szBase64Dec, sizeof(szBase64Dec));
	EXPECT_STREQ("12345678", szBase64Dec);
	LOG_D("base64Dec[%s] len=[%d]", szBase64Dec, nDecLen);

	LOG_FUNC_END();
}

TEST(TestComm, T_Time)
{
	LOG_FUNC_START();

	char	szTemp[128] = { 0 };

	LOG_D("cur time[%s]", STR_GetCurTimeStr(szTemp, sizeof(szTemp), 1));
	EXPECT_EQ((unsigned long)time(NULL), (unsigned long)STR_Str2Time(szTemp, 1));
	LOG_D("cur t=[%#x] get t=[%#x]", (unsigned long)time(NULL), (unsigned long)STR_Str2Time(szTemp, 1));
	BUFCLR(szTemp);
	LOG_D("cur time[%s]", STR_GetCurTimeStr(szTemp, sizeof(szTemp), 2));
	EXPECT_EQ((unsigned long)time(NULL), (unsigned long)STR_Str2Time(szTemp, 2));
	LOG_D("cur t=[%#x] get t=[%#x]", (unsigned long)time(NULL), (unsigned long)STR_Str2Time(szTemp, 2));
	BUFCLR(szTemp);
	LOG_D("cur time[%s]", STR_GetCurTimeStr(szTemp, sizeof(szTemp), 3));
	EXPECT_EQ((unsigned long)time(NULL), (unsigned long)STR_Str2Time(szTemp, 3));
	LOG_D("cur t=[%#x] get t=[%#x]", (unsigned long)time(NULL), (unsigned long)STR_Str2Time(szTemp, 3));

	LOG_FUNC_END();
}

TEST(TestComm, T_BINHEX)
{
	LOG_FUNC_START();

	char			szTemp[128] = { 0 };
	unsigned char	bTemp[1024] = { 0 };
	int				len = 0;

	bTemp[0] = 0x00;
	bTemp[1] = 0x84;
	bTemp[2] = 0x00;
	bTemp[3] = 0x00;
	bTemp[4] = 0x08;
	
	STR_BIN2HEX(bTemp, 5, sizeof(szTemp), szTemp);
	EXPECT_STREQ(szTemp, "0084000008");
	EXPECT_EQ(-1, STR_BIN2HEX(NULL, 0, 1, NULL));
	EXPECT_EQ(-1, STR_BIN2HEX(bTemp, 5, 5, szTemp));

	sprintf(szTemp, "313233343536616263646566414243444546");
	len = STR_HEX2BIN(szTemp, bTemp, sizeof(bTemp));
	EXPECT_EQ(18, len);
	EXPECT_STREQ((char*)bTemp, "123456abcdefABCDEF");
	LOGBIN_D(bTemp, len);
	EXPECT_EQ(-1, STR_HEX2BIN(szTemp, bTemp, 5));

	sprintf(szTemp, "00A4cDEf023f00");
	len = STR_HEX2BIN(szTemp, bTemp, sizeof(bTemp));
	EXPECT_EQ(7, len);

	sprintf((char*)bTemp, "12345678");
	STR_Reverse(bTemp, 8);
	EXPECT_STREQ((char*)bTemp, "87654321");

	LOG_FUNC_END();
}

TEST(TestComm, T_Mem)
{
	int				i;
	unsigned char	*p[300];

	LOG_FUNC_START();

	MyMem_SetDbg(123);

	for (i = 0; i < 300; i++)
	{
		p[i] = (unsigned char*)Mem_Malloc(100);
	}

	p[10] = (unsigned char*)Mem_Realloc(p[10], 120);

	for (i = 0; i < 300; i++)
	{
		Mem_Free(p[i]);
	}

	p[0] = (unsigned char*)Mem_Malloc(0);
	ASSERT_EQ(NULL, p[0]);

	//p[0] = (unsigned char*)Mem_Malloc(100);	//for leak

	LOG_FUNC_END();
}




//运行该文件需要gtest库和odbc库的支持
//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/projects/DProcLib_linux/DProcLib_2015/debug_bin
//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/odbc/lib
//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/dip/depend/odbc/mysql
//export ODBCSYSINI=~/dip/depend/odbc/etc
int main(int argc, char* argv[])
{
	int		ret;

	LOG_Init(LOG_LEV_DEBUG, LOG_PRIFX, LOG_FILE);
	Mem_SetDbgOn();
	testing::InitGoogleTest(&argc, argv);

	ret = RUN_ALL_TESTS();

	Test_Other();

	Mem_Leak();
	LOG_UnInit();
#ifdef _WINDOWS
	system("pause");
#endif
	return ret;
}