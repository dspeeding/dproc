#pragma once

#define MAX_SCHEMA_NUM		100		//最大schema个数
#define MAX_TAB_NUM			10000	//最大表个数
#define MAX_COL_NUM			1000	//最大列个数

#define MAX_TRANS_NUM		10000	//最大同时事物个数

typedef struct LC_INFO_ST
{
	uint64_t	scn;
	char		time[32];
	uint32_t	thread;
	uint64_t	xusn;
	uint64_t	xslt;
	uint64_t	xsqn;
	uint64_t	xid;
	uint32_t	rbasqn;
	uint64_t	rbablk;
	uint32_t	rbabyte;
	uint32_t	objn;
	char		owner[128];
	char		objname[128];
	int			objnm_ind;
	uint32_t	seg_type;
	char		rowid[128];
	uint32_t	rollback;
	uint32_t	opc;
	char*		sql_redo;
	char		rsid[128];
	uint64_t	seq;
	uint32_t	ssn;
	uint32_t	csf;
	char		info[40];
}LC_INFO;

typedef struct REC_INFO_ST {
	LC_INFO		info;
	char		*sql_anl;
	struct REC_INFO_ST* next;
}REC_INFO;

typedef struct TRANS_ST
{
	uint32_t	flg;
	uint32_t	sct;
	uint64_t	xid;
	uint64_t	sscn;
	uint64_t	scn;
	int			ct;
	REC_INFO	node;
	struct TRANS_ST* next;
}TRANS;


