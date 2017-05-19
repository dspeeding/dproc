#pragma once

#include <sql.h>
#include <sqlext.h>


#ifdef __cplusplus
extern "C"
{
#endif

#define CHECK_ERR_ENV(rc)	odbc_exec(ctx->henv, SQL_HANDLE_ENV, rc)
#define CHECK_ERR_DBC(rc)	odbc_exec(ctx->hdbc, SQL_HANDLE_DBC, rc)
#define CHECK_ERR_STMT(rc)	odbc_exec(stmt->hstmt, SQL_HANDLE_STMT, rc)

typedef struct ODBC_CTX_ST
{
	SQLHENV		henv;
	SQLHDBC		hdbc;
}ODBC_CTX;


typedef struct ODBC_STMT
{
	int			flag;
	SQLHSTMT	hstmt;
}ODBC_STMT;


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
int odbc_connect(OUT ODBC_CTX* ctx, IN const char* conn_str, IN int timeout, IN int auto_commit_flag);

/*************************************************************************
@Purpose :	�Ͽ����ݿ⣬�ͷ��ڴ�
@Param   :	ctx			-- ���ݿ����Ӿ��
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void odbc_disconnect(IN ODBC_CTX* ctx);

/*************************************************************************
@Purpose :	׼��sql���
@Param   :	ctx			-- ���ݿ����Ӿ��
@Param   :	stmt		-- stment
@Param   :	sql			-- sql���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	�����һ�ε��øú���֮ǰstmt��Ҫmemset����
*************************************************************************/
int odbc_prepare_stmt(IN ODBC_CTX* ctx, OUT ODBC_STMT* stmt, IN const char* sql);

/*************************************************************************
@Purpose :	�ͷ�stmt
@Param   :	stmt		-- ���ݿ�stmt���
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void odbc_close_stmt(IN ODBC_STMT* stmt);

/*************************************************************************
@Purpose :	ִ��sql���
@Param   :	stmt		-- ���ݿ�stmt���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int odbc_execute(IN ODBC_STMT* stmt);

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
int odbc_bind_col(IN ODBC_STMT* stmt, IN int pos, IN int vtype, int ptype, unsigned long csz, int dec, void* value, int len, int* psi);

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
int odbc_define_col(IN ODBC_STMT* stmt, IN int pos, IN int target_data_type, IN void* target_val, IN int max_val_len);

/*************************************************************************
@Purpose :	fetch
@Param   :	stmt				-- stmt���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int odbc_fetch(IN ODBC_STMT* stmt);

/*************************************************************************
@Purpose :	end trans
@Param   :	stmt				-- stmt���
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int odbc_end_trans(IN ODBC_CTX* ctx, IN int how);


#ifdef __cplusplus
}
#endif



