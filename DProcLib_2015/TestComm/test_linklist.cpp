#include "basetype.h"
#include "gtest/gtest.h"
#include "test_sqlist.h"

class LinkListTest : public testing::Test
{
public:
	int			flag;
	PLinkList	pHead;

	virtual void SetUp()
	{
		int		nRet;

		LOG_D("SetUp ...");
		flag = 0;
		nRet = InitLinkList(&pHead, sizeof(STU));
		if (nRet != ERR_OK)
		{
			LOG_E("InitLinkList err[%#x]", nRet);
			return;
		}
		flag = 1;
	}

	virtual void TearDown()
	{
		LOG_D("TearDown ...");
		if (flag == 1)
		{
			DestroyLinkList(pHead);
		}
	}
};

static int visit(void* val)
{
	STU*	stu = (STU*)val;
	LOG_D("stu id=[%d] name=[%s]", stu->id, stu->name);
	return ERR_OK;
}

static int visit_Except(void* val)
{
	STU*	stu = (STU*)val;
	LOG_D("stu id=[%d] name=[%s]", stu->id, stu->name);
	return ERR_COMM_PARAM;
}

static int compare(void* val1, void* val2)
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

TEST_F(LinkListTest, T_LinkList_Insert)
{
	int			i;
	int			nRet;
	STU			stu;

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertLinkList(pHead, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	TraverseLinkList(pHead, visit, 0);
	LOG_D("-------------------------");

	for (i = 5; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertLinkList(pHead, 0, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	TraverseLinkList(pHead, visit, 0);
	LOG_D("-------------------------");
}

TEST_F(LinkListTest, T_LinkList_GetData)
{
	int			i;
	int			nRet;
	STU			stu;
	char		temp[128] = { 0 };

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertLinkList(pHead, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 5; i++)
	{
		nRet = GetLinkListElem(pHead, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		EXPECT_EQ(stu.id, i) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", i);
		EXPECT_STREQ(temp, stu.name) << "index=[" << i << "]";
		visit(&stu);
	}
}

TEST_F(LinkListTest, T_LinkList_DelData)
{
	int			i;
	int			nRet;
	STU			stu;
	char		temp[128] = { 0 };

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertLinkList(pHead, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 5; i++)
	{
		nRet = DeleteLinkList(pHead, 0, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		visit(&stu);
		ASSERT_EQ(i, stu.id) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", i);
		ASSERT_STREQ(temp, stu.name) << "index=[" << i << "]";
	}

	for (i = 5; i < 10; i++)
	{
		nRet = DeleteLinkList(pHead, 9-i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		visit(&stu);
		ASSERT_EQ(9-i+5, stu.id) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", 9 - i + 5);
		ASSERT_STREQ(temp, stu.name) << "index=[" << i << "]";
	}

	EXPECT_EQ(0, GetLinkListLength(pHead));
	ClearLinkList(pHead);
}

TEST_F(LinkListTest, T_LinkList_UpdData)
{
	int			i;
	int			nRet;
	STU			stu;
	char		temp[128] = { 0 };

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertLinkList(pHead, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		nRet = GetLinkListElem(pHead, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		sprintf(stu.name, "upd_hah_%02d", i);
		nRet = UpdateLinkList(pHead, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	EXPECT_EQ(5, GetLinkListLength(pHead));
	for (i = 0; i < 5; i++)
	{
		nRet = DeleteLinkList(pHead, 0, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		visit(&stu);
		ASSERT_EQ(i, stu.id) << "index=[" << i << "]";
		sprintf(temp, "upd_hah_%02d", i);
		ASSERT_STREQ(temp, stu.name) << "index=[" << i << "]";
	}
}

TEST_F(LinkListTest, T_LinkList_ClearData)
{
	int			i;
	int			nRet;
	STU			stu;

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertLinkList(pHead, i, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	EXPECT_EQ(5, GetLinkListLength(pHead));
	ClearLinkList(pHead);
	EXPECT_EQ(0, GetLinkListLength(pHead));

}

TEST_F(LinkListTest, T_LinkList_Sort)
{
	int			i;
	int			nRet;
	STU			stu;
	char		temp[1024] = { 0 };
	int			nPos = 0;

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = InsertLinkList(pHead, 0, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	EXPECT_EQ(5, GetLinkListLength(pHead));
	TraverseLinkList(pHead, visit, 0);
	LOG_D("----------");

	nRet = SortLinkList(pHead, compare);
	ASSERT_EQ(nRet, ERR_OK);

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		nRet = GetLinkListElem(pHead, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		EXPECT_EQ(stu.id, i) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", i);
		EXPECT_STREQ(temp, stu.name) << "index=[" << i << "]";
		visit(&stu);
	}
	LOG_D("----------");

	nRet = ReverseLinkList(pHead);
	ASSERT_EQ(nRet, ERR_OK);

	for (i = 0; i < 5; i++)
	{
		memset(&stu, 0, sizeof(stu));
		nRet = GetLinkListElem(pHead, i, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		EXPECT_EQ(stu.id, 4-i) << "index=[" << i << "]";
		sprintf(temp, "hah_%02d", 4-i);
		EXPECT_STREQ(temp, stu.name) << "index=[" << i << "]";
		visit(&stu);
	}
	LOG_D("----------");

	stu.id = 2;
	sprintf(stu.name, "hah_%02d", 2);
	nRet = LocateLinkListElem(pHead, &stu, compare, &nPos);
	ASSERT_EQ(nRet, ERR_OK);
	EXPECT_EQ(nPos, 2);
}

TEST_F(LinkListTest, T_LinkList_Except)
{
	int			nRet;
	STU			stu;

	DestroyLinkList(pHead);

	nRet = InitLinkList(&pHead, 2);
	ASSERT_EQ(nRet, ERR_COMM_PARAM);

	nRet = InitLinkList(&pHead, sizeof(STU));
	ASSERT_EQ(nRet, ERR_OK);

	memset(&stu, 0, sizeof(stu));
	stu.id = 0;
	sprintf(stu.name, "hah_%02d", 0);
	nRet = InsertLinkList(pHead, 1, (void*)&stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	nRet = InsertLinkList(pHead, 0, (void*)&stu);
	ASSERT_EQ(nRet, ERR_OK);

	TraverseLinkList(pHead, visit, 0);
	LOG_D("----------");

	ASSERT_EQ(1, GetLinkListLength(pHead));

	nRet = GetLinkListElem(pHead, 1, &stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	nRet = DeleteLinkList(pHead, 1, &stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	nRet = UpdateLinkList(pHead, 1, &stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);

	memset(&stu, 0, sizeof(stu));
	stu.id = 1;
	sprintf(stu.name, "hah_%02d", 1);
	nRet = InsertLinkList(pHead, 0, (void*)&stu);
	ASSERT_EQ(nRet, ERR_OK);

	TraverseLinkList(pHead, visit_Except, 1);
}