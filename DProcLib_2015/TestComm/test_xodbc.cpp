#include "basetype.h"
#include "gtest/gtest.h"
#include "test_sqlist.h"
#include "xodbc/xodbc.h"

// create table test1(id int, name varchar(128), d datetime);
TEST(Test_Xodbc, T_CONN)
{
	char		conn_str[1024];
	ODBC_CTX	octx;
	ODBC_CTX*	ctx = &octx;
	ODBC_STMT	ostmt;
	ODBC_STMT*	stmt = &ostmt;
	char		sql[1024] = { 0 };
	int			ct = 0;
	

	memset(&octx, 0, sizeof(octx));
	memset(&ostmt, 0, sizeof(ostmt));
#ifdef _WINDOWS
	sprintf(conn_str, "Driver={MySQL ODBC 5.3 ANSI Driver};Server=172.16.1.198;Database=test;Port=3306;Uid=r7;Pwd=123456;Charset=utf8");
#else
	sprintf(conn_str, "Driver=mysqldriver;Server=172.16.1.200;Database=dip;Port=3306;Uid=r7;Pwd=r7;Charset=utf8");
#endif
	
	ASSERT_EQ(odbc_connect(ctx, conn_str, 5, 0), ERR_OK);
	LOG_D("connect success...");
	//ASSERT_EQ(odbc_set_autocommit(ctx, 0), ERR_OK);

	sprintf(sql, "select count(*) from test1");

	ASSERT_EQ(odbc_prepare_stmt(ctx, stmt, sql), ERR_OK);

	ASSERT_EQ(odbc_define_col(stmt, 1, SQL_C_LONG, &ct, sizeof(int)), ERR_OK);

	ASSERT_EQ(odbc_execute(stmt), ERR_OK);

	ASSERT_EQ(odbc_fetch(stmt), ERR_OK);

	if (ct > 50)
	{
		odbc_close_stmt(stmt);
		odbc_disconnect(ctx);
		return;
	}

	sprintf(sql, "insert into test1(id,name,d) values(0,'abc','20170417')");
	odbc_close_stmt(stmt);

	ASSERT_EQ(odbc_prepare_stmt(ctx, stmt, sql), ERR_OK);

	ASSERT_EQ(odbc_execute(stmt), ERR_OK);
	odbc_close_stmt(stmt);
	
	ASSERT_EQ(odbc_end_trans(ctx, SQL_COMMIT), ERR_OK);

	odbc_disconnect(ctx);
}

TEST(Test_Xodbc, T_SELECT)
{
	int			ret;
	char		conn_str[1024];
	ODBC_CTX	octx;
	ODBC_CTX*	ctx = &octx;
	ODBC_STMT	ostmt;
	ODBC_STMT*	stmt = &ostmt;
	char		sql[1024] = { 0 };
	int			id = 0;
	char		name[128] = { 0 };
	char		time[128] = { 0 };
	int			id_a = 0;
	char		name_a[512] = { 0 };
	int			ind_a = SQL_NTS;


	memset(&octx, 0, sizeof(octx));
	memset(&ostmt, 0, sizeof(ostmt));
#ifdef _WINDOWS
	sprintf(conn_str, "Driver={MySQL ODBC 5.3 ANSI Driver};Server=172.16.1.198;Database=test;Port=3306;Uid=r7;Pwd=123456;Charset=utf8");
#else
	sprintf(conn_str, "Driver=mysqldriver;Server=172.16.1.200;Database=dip;Port=3306;Uid=r7;Pwd=r7;Charset=utf8");
#endif
	ASSERT_EQ(odbc_connect(ctx, conn_str, 5, 1), ERR_OK);

	sprintf(sql, "select id,name,d from test1 where id=? and name=?");

	ASSERT_EQ(odbc_prepare_stmt(ctx, stmt, sql), ERR_OK);

	id_a = 0;
	strcpy(name_a, "abc");
	ASSERT_EQ(odbc_bind_col(stmt, 1, SQL_C_LONG, SQL_INTEGER, 0, 0, &id_a, sizeof(int), NULL), ERR_OK);
	ASSERT_EQ(odbc_bind_col(stmt, 2, SQL_C_CHAR, SQL_CHAR, 128, 0, name_a, strlen(name_a)+1, &ind_a), ERR_OK);

	ASSERT_EQ(odbc_define_col(stmt, 1, SQL_C_LONG, &id, sizeof(int)), ERR_OK);
	ASSERT_EQ(odbc_define_col(stmt, 2, SQL_C_CHAR, name, sizeof(name)), ERR_OK);
	ASSERT_EQ(odbc_define_col(stmt, 3, SQL_C_CHAR, time, sizeof(time)), ERR_OK);

	ASSERT_EQ(odbc_execute(stmt), ERR_OK);
	while (1)
	{
		ret = odbc_fetch(stmt);
		if (ret == ERR_XODBC_NO_DATA)
		{
			break;
		}
		ASSERT_EQ(ret, ERR_OK);

		LOG_D("id[%d] name[%s] time[%s]", id, name, time);
	}
	odbc_close_stmt(stmt);

	odbc_disconnect(ctx);
}