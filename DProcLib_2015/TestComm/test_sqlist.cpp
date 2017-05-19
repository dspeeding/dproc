#include "basetype.h"
#include "gtest/gtest.h"
#include "test_sqlist.h"

class DSListTest : public testing::Test
{
public:
	int			flag;
	SqList*		L;

	virtual void SetUp()
	{
		int		nRet;

		LOG_D("SetUp ...");
		flag = 0;
		nRet = InitList(&L, sizeof(STU), 10);
		if (nRet != ERR_OK)
		{
			LOG_E("InitList err[%#x]", nRet);
			return;
		}
		flag = 1;
	}

	virtual void TearDown()
	{
		LOG_D("TearDown ...");
		if (flag == 1)
		{
			DestroyList(L);
		}
	}
};

int visit(void* val)
{
	STU*	stu = (STU*)val;
	LOG_D("stu id=[%d] name=[%s]", stu->id, stu->name);
	return ERR_OK;
}

int visit_Except(void* val)
{
	STU*	stu = (STU*)val;
	LOG_D("stu id=[%d] name=[%s]", stu->id, stu->name);
	return ERR_COMM_PARAM;
}

int compare(void* val1, void* val2)
{
	STU*	stu1 = (STU*)val1;
	STU*	stu2 = (STU*)val2;

	if (stu1->id > stu2->id)
	{
		return 1;
	}
	if (stu1->id == stu2->id)
	{
		return strcmp(stu1->name, stu2->name);
	}
	if (stu1->id < stu2->id)
	{
		return -1;
	}

	return 0;
}


TEST_F(DSListTest, T_DS_Insert)
{
	int			nRet;
	int			i;
	STU			stu;

	ClearList(L);

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	TraverseList(L, visit, 0);
	LOG_D("-------------------------");

	for (i = 5; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, 0, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	TraverseList(L, visit, 0);
	LOG_D("-------------------------");

	for (i = 10; i < 15; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	TraverseList(L, visit, 0);
	LOG_D("-------------------------");

}

TEST_F(DSListTest, T_DS_GetData)
{
	int			nRet;
	int			i;
	STU			stu;
	char		temp[128] = { 0 };

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 5; i++)
	{
		nRet = GetElem(L, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		EXPECT_EQ(stu.id, i) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", i);
		EXPECT_STREQ(temp, stu.name) << "index=[" << i << "]";
		visit(&stu);
	}

	for (i = 5; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, 0, (void*)&stu);	//here is 0
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 5; i++)
	{
		nRet = GetElem(L, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		EXPECT_EQ(stu.id, 9-i) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", 9-i);
		EXPECT_STREQ(temp, stu.name) << "index=[" << i << "]";
		visit(&stu);
	}
}

TEST_F(DSListTest, T_DS_DelData)
{
	int			nRet;
	int			i;
	STU			stu;
	char		temp[128] = { 0 };

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 10; i++)
	{
		nRet = DeleteList(L, 0, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		visit(&stu);
		ASSERT_EQ(i, stu.id) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", i);
		ASSERT_STREQ(temp, stu.name) << "index=[" << i << "]";
	}
}

TEST_F(DSListTest, T_DS_UpdateData)
{
	int			nRet;
	int			i;
	STU			stu;
	char		temp[128] = { 0 };

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, 0, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}
	TraverseList(L, visit, 0);
	LOG_D("----------");

	for (i = 0; i < 5; i++)
	{
		nRet = GetElem(L, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		sprintf(stu.name, "upd_haha_%02d", 4 - i);
		nRet = UpdateList(L, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	TraverseList(L, visit, 0);
	LOG_D("----------");

	nRet = SortList(L, compare);
	ASSERT_EQ(nRet, ERR_OK);

	for (i = 0; i < 5; i++)
	{
		nRet = GetElem(L, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		visit(&stu);
		ASSERT_EQ(i, stu.id) << "index=[" << i << "]";
		sprintf(temp, "upd_haha_%02d", i);
		ASSERT_STREQ(temp, stu.name) << "index=[" << i << "]";
	}
}

TEST_F(DSListTest, T_DS_ClearData)
{
	int			nRet;
	int			i;
	STU			stu;


	ASSERT_EQ(1, IsListEmpty(L));

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertList(L, 0, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	TraverseList(L, visit, 0);
	LOG_D("=========before clear==========");

	ASSERT_EQ(10, GetListLength(L));

	if (IsListEmpty(L) == 0)
	{
		LOG_D("list is not empty,len=[%d]", GetListLength(L));
		ClearList(L);
	}

	LOG_D("list cleared,len=[%d]", GetListLength(L));
	ASSERT_EQ(0, GetListLength(L));
}

TEST_F(DSListTest, T_DS_Except)
{
	int			nRet;
	STU			stu;

	DestroyList(L);

	nRet = InitList(&L, sizeof(STU), 0);
	ASSERT_EQ(nRet, ERR_COMM_PARAM);

	nRet = InitList(&L, sizeof(STU), 5);
	ASSERT_EQ(nRet, ERR_OK);

	memset(&stu, 0, sizeof(stu));
	stu.id = 0;
	sprintf(stu.name, "hah_%02d", 0);
	nRet = InsertList(L, 1, (void*)&stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	nRet = InsertList(L, 0, (void*)&stu);
	ASSERT_EQ(nRet, ERR_OK);

	TraverseList(L, visit, 0);
	LOG_D("----------");

	ASSERT_EQ(1, GetListLength(L));

	nRet = GetElem(L, 1, &stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	nRet = DeleteList(L, 1, &stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	nRet = UpdateList(L, 1, &stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	memset(&stu, 0, sizeof(stu));
	stu.id = 1;
	sprintf(stu.name, "hah_%02d", 1);
	nRet = InsertList(L, 0, (void*)&stu);
	ASSERT_EQ(nRet, ERR_OK);
	
	TraverseList(L, visit_Except, 1);
}
