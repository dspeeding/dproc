#include "basetype.h"
#include "xodbc/xodbc.h"

#ifdef _WINDOWS
#pragma comment(lib, "odbc32.lib")
#endif

/*************************************************************************
@Purpose :	��ȡ������Ϣ
@Param   :	h			-- ���ݿ��� �� henv hdbc
@Param   :	ht			-- ������� �� SQL_HANDLE_ENV SQL_HANDLE_DBC
@Return  :	rc
@Modify  :	
@Remark  :	���rc�д��� ���ӡ���� ��ԭ�ⲻ������rc
*************************************************************************/
static int odbc_exec(IN SQLHANDLE h, IN SQLSMALLINT ht, IN SQLRETURN rc)
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

/*************************************************************************
@Purpose :	�������ݿ�
@Param   :	ctx				-- ���ݿ����Ӿ��
@Param   :	conn_str		-- ���ݿ������ַ���
@Param   :	timeout			-- ���ӳ�ʱʱ�䣬��λ��
@Param   :	auto_commit_flag-- �Ƿ��Զ��ύ 0--�ֶ��ύ ����--�Զ��ύ
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  : conn_str ������ʾ
			Driver={MySQL ODBC 5.3 ANSI Driver};Server=172.16.1.200;Database=dip;Port=3306;Uid=r7;Pwd=r7;Charset=utf8
*************************************************************************/
int odbc_connect(OUT ODBC_CTX* ctx, IN const char* conn_str, IN int timeout, IN int auto_commit_flag)
{
	SQLRETURN	ret = 0;
	char		conn_out[1024] = { 0 };
	SQLLEN		attr;

	if (timeout <= 0)
	{
		LOG_E("param timeout is wrong, timeout=[%d]", timeout);
		return ERR_COMM_PARAM;
	}
	if (auto_commit_flag)
	{
		attr = SQL_AUTOCOMMIT_ON;
	}
	else
	{
		attr = SQL_AUTOCOMMIT_OFF;
	}

	//sprintf(conn_str, "Driver={MySQL ODBC 5.3 ANSI Driver};Server=172.16.1.200;Database=dip;Port=3306;Uid=r7;Pwd=r7;Charset=utf8");

	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, (SQLHANDLE*)&ctx->henv);
	if (ret != SQL_SUCCESS)
	{
		LOG_E("sql alloc env error");
		return ERR_XODBC_ALLOC_HANDLE;
	}

	ret = CHECK_ERR_ENV(SQLSetEnvAttr(ctx->henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("set env attr version error");
		SQLFreeHandle(SQL_HANDLE_ENV, ctx->henv);
		return ERR_XODBC_SET_ATTR;
	}

	ret = CHECK_ERR_ENV(SQLAllocHandle(SQL_HANDLE_DBC, ctx->henv, (SQLHANDLE*)&ctx->hdbc));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("sql alloc dbc error");
		SQLFreeHandle(SQL_HANDLE_ENV, ctx->henv);
		return ERR_XODBC_ALLOC_HANDLE;
	}

	ret = CHECK_ERR_DBC(SQLSetConnectAttr(ctx->hdbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER)(SQLLEN)timeout, SQL_IS_INTEGER));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLSetConnectAttr timeout error");
		SQLFreeHandle(SQL_HANDLE_DBC, ctx->hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, ctx->henv);
		return ERR_XODBC_SET_ATTR;
	}

	ret = CHECK_ERR_DBC(SQLSetConnectAttr(ctx->hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)attr, SQL_IS_INTEGER));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLSetConnectAttr autocommit error");
		SQLFreeHandle(SQL_HANDLE_DBC, ctx->hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, ctx->henv);
		return ERR_XODBC_SET_ATTR;
	}

	ret = CHECK_ERR_DBC(SQLDriverConnectA(ctx->hdbc, NULL, (SQLCHAR *)conn_str, SQL_NTS, (SQLCHAR *)conn_out, sizeof(conn_out), NULL, SQL_DRIVER_NOPROMPT));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLDriverConnectA error");
		SQLFreeHandle(SQL_HANDLE_DBC, ctx->hdbc);
		SQLFreeHandle(SQL_HANDLE_ENV, ctx->henv);
		return ERR_XODBC_CONNECT;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	�Ͽ����ݿ⣬�ͷ��ڴ�
@Param   :	ctx			-- ���ݿ����Ӿ��
@Return  :	None
@Modify  :
@Remark  : 
*************************************************************************/
void odbc_disconnect(IN ODBC_CTX* ctx)
{
	if (ctx)
	{
		if (ctx->hdbc)
		{
			SQLDisconnect(ctx->hdbc);
			SQLFreeHandle(SQL_HANDLE_DBC, ctx->hdbc);
		}
		if (ctx->henv)
		{
			SQLFreeHandle(SQL_HANDLE_ENV, ctx->henv);
		}
	}
}

/*************************************************************************
@Purpose :	׼��sql���
@Param   :	ctx			-- ���ݿ����Ӿ��
@Param   :	stmt		-- stment
@Param   :	sql			-- sql���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	�����һ�ε��øú���֮ǰstmt��Ҫmemset����
*************************************************************************/
int odbc_prepare_stmt(IN ODBC_CTX* ctx, OUT ODBC_STMT* stmt, IN const char* sql)
{
	int		ret;

	if (stmt->hstmt == NULL)
	{
		ret = CHECK_ERR_DBC(SQLAllocHandle(SQL_HANDLE_STMT, ctx->hdbc, (SQLHANDLE*)&stmt->hstmt));
		if (ret != SQL_SUCCESS)
		{
			LOG_E("SQLAllocHandle STMT error");
			return ERR_XODBC_ALLOC_HANDLE;
		}
	}

	ret = CHECK_ERR_STMT(SQLPrepareA(stmt->hstmt, (SQLCHAR *)sql, SQL_NTS));
	if (ret != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, stmt->hstmt);
		memset(stmt, 0, sizeof(ODBC_STMT));
		LOG_E("SQLPrepareA sql error");
		return ERR_XODBC_ALLOC_HANDLE;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	�ͷ�stmt
@Param   :	stmt		-- ���ݿ�stmt���
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void odbc_close_stmt(IN ODBC_STMT* stmt)
{
	if (stmt->hstmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, stmt->hstmt);
		stmt->hstmt = NULL;
	}
}

/*************************************************************************
@Purpose :	ִ��sql���
@Param   :	stmt		-- ���ݿ�stmt���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	
*************************************************************************/
int odbc_execute(IN ODBC_STMT* stmt)
{
	int		ret;

	CHECK_POINTER(stmt, ERR_COMM_NULL_POINTER);

	ret = CHECK_ERR_STMT(SQLExecute(stmt->hstmt));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLExecute error");
		return ERR_XODBC_EXECUTE;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	���ð���
@Param   :	stmt			-- stmt���
@Param   :	pos				-- Ҫ�󶨵��к�
@Param   :	vtype			-- �󶨵�C������������
@Param	 :	ptype			-- �󶨵�SQL��������
@Param	 :	csz				-- SQL�ж�����ֶεĳ���
@Param	 :	dec				-- С��λ
@Param	 :	value			-- �󶨵Ļ�����
@Param	 :	len				-- �󶨵Ļ������ĳ���
@Param	 :	psi				-- �����������Ĳ����ĳ��� 
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	pos һ���1��ʼ  sql�����ռλ��ʱʹ��
			psi ������ַ�����ΪSQL_NTS ��ʾ���������һ��0��β���ַ���
*************************************************************************/
int odbc_bind_col(IN ODBC_STMT* stmt, IN int pos, IN int vtype, int ptype, unsigned long csz, int dec, void* value, int len, int* psi)
{
	int		ret;

	CHECK_POINTER(stmt, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(value, ERR_COMM_NULL_POINTER);

	ret = CHECK_ERR_STMT(SQLBindParameter(stmt->hstmt, pos, SQL_PARAM_INPUT, vtype, ptype, csz, dec, value, len, (SQLLEN *)psi));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLBindParameter error");
		return ERR_XODBC_BIND_COL;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	����define��
@Param   :	stmt				-- stmt���
@Param   :	pos					-- Ҫ�󶨵��к�
@Param   :	target_data_type	-- �󶨵�C������������
@Param	 :	target_val			-- �󶨵Ļ�����
@Param	 :	max_val_len			-- �󶨵Ļ�������󳤶�
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	pos һ���1��ʼ
*************************************************************************/
int odbc_define_col(IN ODBC_STMT* stmt, IN int pos, IN int target_data_type, IN void* target_val, IN int max_val_len)
{
	int		ret;
	SQLLEN	len;

	CHECK_POINTER(stmt, ERR_COMM_NULL_POINTER);
	CHECK_POINTER(target_val, ERR_COMM_NULL_POINTER);

	ret = CHECK_ERR_STMT(SQLBindCol(stmt->hstmt, pos, target_data_type, target_val, max_val_len, &len));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLBindCol error");
		return ERR_XODBC_DEFINE_COL;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	fetch
@Param   :	stmt				-- stmt���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	
*************************************************************************/
int odbc_fetch(IN ODBC_STMT* stmt)
{
	int		ret;

	CHECK_POINTER(stmt, ERR_COMM_NULL_POINTER);

	ret = CHECK_ERR_STMT(SQLFetch(stmt->hstmt));
	if (ret != SQL_SUCCESS)
	{
		if (ret == SQL_NO_DATA)
		{
			return ERR_XODBC_NO_DATA;
		}
		LOG_E("SQLFetch error");
		return ERR_XODBC_FETCH;
	}

	return ERR_OK;
}

/*************************************************************************
@Purpose :	end trans
@Param   :	stmt				-- stmt���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int odbc_end_trans(IN ODBC_CTX* ctx, IN int how)
{
	int		ret;

	CHECK_POINTER(ctx, ERR_COMM_NULL_POINTER);
	if ((how != SQL_COMMIT) && (how != SQL_ROLLBACK))
	{
		LOG_E("param how error,how=[%d]", how);
		return ERR_COMM_PARAM;
	}

	ret = CHECK_ERR_DBC(SQLEndTran(SQL_HANDLE_DBC, ctx->hdbc, how));
	if (ret != SQL_SUCCESS)
	{
		LOG_E("SQLEndTran error");
		return ERR_XODBC_ENDTRAN;
	}

	return ERR_OK;
}


