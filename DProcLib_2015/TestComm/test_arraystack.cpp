#include "basetype.h"
#include "gtest/gtest.h"
#include "test_sqlist.h"

class ArrayStackTest : public testing::Test
{
public:
	int			flag;
	PArrayStack	pStack;

	virtual void SetUp()
	{
		int		nRet;

		LOG_D("SetUp ...");
		flag = 0;
		nRet = InitArrayStack(&pStack, sizeof(STU), 5);
		if (nRet != ERR_OK)
		{
			LOG_E("InitArrayStack err[%#x]", nRet);
			return;
		}
		flag = 1;
	}

	virtual void TearDown()
	{
		LOG_D("TearDown ...");
		if (flag == 1)
		{
			DestroyArrayStack(pStack);
		}
	}
};

TEST_F(ArrayStackTest, T_ArrayStack_Opt)
{
	int			i;
	int			nRet;
	STU			stu;
	STU			stu1;

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = PushArrayStack(pStack, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		memset(&stu1, 0, sizeof(stu1));
		nRet = GetTopArrayStack(pStack, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		EXPECT_EQ(stu.id, 10-1 - i) << "index=[" << i << "]";

		nRet = PopArrayStack(pStack, (void*)&stu1);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
		EXPECT_EQ(stu.id, stu1.id) << "index=[" << i << "]";
	}
}

TEST_F(ArrayStackTest, T_ArrayStack_Except)
{
	int			i;
	int			nRet;
	STU			stu;

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		stu.id = i;
		sprintf(stu.name, "hah_%02d", i);
		nRet = PushArrayStack(pStack, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	for (i = 0; i < 10; i++)
	{
		memset(&stu, 0, sizeof(stu));
		nRet = PopArrayStack(pStack, (void*)&stu);
		ASSERT_EQ(nRet, ERR_OK) << "index=[" << i << "]";
	}

	nRet = GetTopArrayStack(pStack, (void*)&stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);
	nRet = PopArrayStack(pStack, (void*)&stu);
	ASSERT_EQ(nRet, ERR_COMM_OUT_OF_INDEX);
}