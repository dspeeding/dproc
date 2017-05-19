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
@Purpose :	连接数据库
@Param   :	ctx				-- 数据库连接句柄
@Param   :	conn_str		-- 数据库连接字符串
@Param   :	timeout			-- 连接超时时间，单位秒
@Param   :	auto_commit_flag-- 是否自动提交 0--手动提交 其他--自动提交
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  : conn_str 如下所示
Driver={MySQL ODBC 5.3 ANSI Driver};Server=172.16.1.200;Database=dip;Port=3306;Uid=r7;Pwd=r7;Charset=utf8
*************************************************************************/
int odbc_connect(OUT ODBC_CTX* ctx, IN const char* conn_str, IN int timeout, IN int auto_commit_flag);

/*************************************************************************
@Purpose :	断开数据库，释放内存
@Param   :	ctx			-- 数据库连接句柄
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void odbc_disconnect(IN ODBC_CTX* ctx);

/*************************************************************************
@Purpose :	准备sql语句
@Param   :	ctx			-- 数据库连接句柄
@Param   :	stmt		-- stment
@Param   :	sql			-- sql语句
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	这里第一次调用该函数之前stmt需要memset清零
*************************************************************************/
int odbc_prepare_stmt(IN ODBC_CTX* ctx, OUT ODBC_STMT* stmt, IN const char* sql);

/*************************************************************************
@Purpose :	释放stmt
@Param   :	stmt		-- 数据库stmt句柄
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void odbc_close_stmt(IN ODBC_STMT* stmt);

/*************************************************************************
@Purpose :	执行sql语句
@Param   :	stmt		-- 数据库stmt句柄
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int odbc_execute(IN ODBC_STMT* stmt);

/*************************************************************************
@Purpose :	设置绑定列
@Param   :	stmt			-- stmt句柄
@Param   :	pos				-- 要绑定的列号
@Param   :	vtype			-- 绑定的C语言数据类型
@Param	 :	ptype			-- 绑定的SQL数据类型
@Param	 :	csz				-- SQL中定义的字段的长度
@Param	 :	dec				-- 小数位
@Param	 :	value			-- 绑定的缓冲区
@Param	 :	len				-- 绑定的缓冲区的长度
@Param	 :	psi				-- 缓冲区包含的参数的长度
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	pos 一般从1开始  sql语句有占位符时使用
psi 如果是字符串则为SQL_NTS 表示这个参数是一个0结尾的字符串
*************************************************************************/
int odbc_bind_col(IN ODBC_STMT* stmt, IN int pos, IN int vtype, int ptype, unsigned long csz, int dec, void* value, int len, int* psi);

/*************************************************************************
@Purpose :	设置define列
@Param   :	stmt				-- stmt句柄
@Param   :	pos					-- 要绑定的列号
@Param   :	target_data_type	-- 绑定的C语言数据类型
@Param	 :	target_val			-- 绑定的缓冲区
@Param	 :	max_val_len			-- 绑定的缓冲区最大长度
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :	pos 一般从1开始
*************************************************************************/
int odbc_define_col(IN ODBC_STMT* stmt, IN int pos, IN int target_data_type, IN void* target_val, IN int max_val_len);

/*************************************************************************
@Purpose :	fetch
@Param   :	stmt				-- stmt句柄
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int odbc_fetch(IN ODBC_STMT* stmt);

/*************************************************************************
@Purpose :	end trans
@Param   :	stmt				-- stmt句柄
@Return  :	ERR_OK -- success ,other -- fail:errcode
@Modify  :
@Remark  :
*************************************************************************/
int odbc_end_trans(IN ODBC_CTX* ctx, IN int how);


#ifdef __cplusplus
}
#endif



