#include "basetype.h"

#include <sql.h>
#include <sqlext.h>
#pragma comment(lib, "dproclib.lib")
#pragma comment(lib, "odbc32.lib")

#define CHECK_ERR_ENV(rc) odbc_exec(henv, SQL_HANDLE_ENV, rc)
#define CHECK_ERR_DBC(rc) odbc_exec(hdbc, SQL_HANDLE_DBC, rc)

int odbc_exec(SQLHANDLE h, SQLSMALLINT ht, SQLRETURN rc)
{
	SQLSMALLINT	rec = 0;
	SQLINTEGER	ec;
	char		state[8192] = { 0 };
	char		message[8192] = { 0 };

	switch (rc)
	{
	case SQL_SUCCESS:
	case SQL_NEED_DATA:
	case SQL_SUCCESS_WITH_INFO:
	case SQL_NO_DATA:
		return rc;
	}
	if (rc == SQL_INVALID_HANDLE)
	{
		LOG_E("ODBC invalid handle");
		return rc;
	}

	do 
	{
		rc = SQLGetDiagRecA(ht, h, ++rec, (SQLCHAR *)state, &ec, (SQLCHAR*)message, 8192, NULL);
		if ((rc == SQL_ERROR) || (rc == SQL_INVALID_HANDLE))
		{
			break;
		}
		if (rec == 1)
		{
			LOG_E("ODBC error[%s][%s]", state, message);
		}
	} while (rc != SQL_NO_DATA);

	return rc;
}


void test()
{
	SQLRETURN	ret = 0;
	SQLHENV		henv = 0;
	SQLHDBC		hdbc;
	char		conn_str[1024] = { 0 };
	char		conn_out[1024] = { 0 };

	sprintf(conn_str, "Driver={MySQL ODBC 5.3 ANSI Driver};Server=172.16.1.200;Database=dip;Port=3306;Uid=r7;Pwd=r7;Charset=utf8");

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, (SQLHANDLE*)&henv);
	if (ret != SQL_SUCCESS)
	{
		LOG_E("sql alloc env error");
		return;
	}

	ret = CHECK_ERR_ENV(SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("set env attr version error");
		goto err;
	}

	ret = CHECK_ERR_ENV(SQLAllocHandle(SQL_HANDLE_DBC, henv, (SQLHANDLE*)&hdbc));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("sql alloc dbc error");
		goto err;
	}

	ret = CHECK_ERR_DBC(SQLDriverConnectA(hdbc, NULL, conn_str, SQL_NTS, conn_out, 1024, NULL, SQL_DRIVER_NOPROMPT));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLDriverConnectA error");
		goto err;
	}

	LOG_D("conn success ...");

	SQLDisconnect(hdbc);

err:
	if (hdbc)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	}
	if (henv)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

}




int main(int argc, char* argv[])
{

	LOG_Init(LOG_LEV_DEBUG, "testODBC", "testODBC");
	test();



	system("pause");
	return 0;
}