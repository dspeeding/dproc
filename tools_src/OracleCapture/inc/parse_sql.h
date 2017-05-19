#pragma once

#include "dict_opt.h"

#define VECTOR_FLAG_NULL			0x00000001			//column is null
#define VECTOR_FLAG_NOT_NULL		0x00000002			//column is not null, for where clasuse only
#define VECTOR_FLAG_DYNAMIC			0x00000004			//column is dynamic bind in execution


#define MAX_UDT_NULL_NUM		1000				//���UDT���и���

typedef struct UDT_NULL_INFO_ST
{
	int				used;				//ʹ�ñ�־			(��0��δʹ�ã�
	int				col_index;			//��ǰ�е�id����
	int				is_null;			//�Ƿ�ΪNULL 0 ��Ϊ�� 1Ϊ��
}UDT_NULL_INFO;


typedef struct SQL_VECTOR_ST
{
	int						flag;
	DICT_COL*				pcol;
	int						ctype;
	char					cname[128];
	char*					cdata;
	struct SQL_VECTOR_ST*	next;
}SQL_VECTOR;

typedef struct SQL_REC_ST
{
	int					flag;
	char				owner[32];
	char				tabname[32];
	int					opc;
	int					otype;
	uint64_t			xid;
	uint64_t			scn;
	uint64_t			sct;
	uint32_t			thread;
	SQL_VECTOR			old_vec;
	SQL_VECTOR			new_vec;
	SQL_VECTOR			where_vec;
	struct SQL_REC_ST*	next;
}SQL_REC;

typedef struct SQL_PKG_ST
{
	int					flag;
	uint64_t			xid;
	uint32_t			sct;
	uint64_t			sscn;
	uint64_t			scn;
	int					ct;
	SQL_REC				rec;
	struct SQL_PKG_ST*	next;
}SQL_PKG;

//���� insert ���
int parse_sql_insert(LC_INFO* info, char** psql);

int parse_sql_update(LC_INFO* info, char** psql);

int parse_sql_delete(LC_INFO* info, char** psql);

