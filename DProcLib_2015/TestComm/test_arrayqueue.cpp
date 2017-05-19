#include "basetype.h"
#include "gtest/gtest.h"
#include "test_sqlist.h"

class ArrayQueueTest : public testing::Test
{
public:
	int			flag;
	PArrayQueue	pQueue;

	virtual void SetUp()
	{
		int		nRet;

		LOG_D("SetUp ...");
		flag = 0;
		nRet = InitArrayQueue(&pQueue, sizeof(STU), 5);
		if (nRet != ERR_OK)
		{
			LOG_E("InitArrayQueue err[%#x]", nRet);
			return;
		}
		flag = 1;
	}

	virtual void TearDown()
	{
		LOG_D("TearDown ...");
		if (flag == 1)
		{
			DestroyArrayQueue(pQueue);
		}
	}
};

TEST_F(ArrayQueueTest, T_ArrayQueue_Opt)
{
	int			i;
	int			nRet;
	STU			stu;
	STU			stu1;

	for (i = 0; i < 3; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = AddArrayQueue(pQueue, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}
	for (i = 0; i < 14; i++)
	{
		memset(&stu, 0, sizeof(stu));
		nRet = DelArrayQueue(pQueue, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		ASSERT_EQ(stu.id, i);
		memset(&stu, 0, sizeof(stu));
		stu.id = i+3;
		sprintf(stu.name, "hah_%02d", i+3);
		nRet = AddArrayQueue(pQueue, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}
	
	EXPECT_EQ(GetArrayQueueLen(pQueue), 3);
	EXPECT_EQ(IsFullArrayQueue(pQueue), 0);
	EXPECT_EQ(IsEmptyArrayQueue(pQueue), 0);

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = AddArrayQueue(pQueue, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	ClearArrayQueue(pQueue);
	EXPECT_EQ(IsEmptyArrayQueue(pQueue), 1);

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = AddArrayQueue(pQueue, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	i = 0;
	while (IsEmptyArrayQueue(pQueue) == 0)
	{
		memset(&stu, 0, sizeof(stu));
		nRet = PeekArrayQueue(pQueue, &stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		memset(&stu1, 0, sizeof(stu1));
		nRet = DelArrayQueue(pQueue, &stu1);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		ASSERT_EQ(stu.id, stu1.id);
		ASSERT_EQ(stu.id, i++);
	}
}