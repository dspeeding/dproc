#include "basetype.h"
#include "testcap.h"
#include "signal_proc.h"
#include "dict_opt.h"
#include "lmr_opt.h"
#include "parse_sql.h"
#include "tx_opt.h"

uint64_t	g_start_scn = 0;
uint64_t	g_end_scn = 0;
DICT_LIST*	g_dict;
DICT_TAB*	g_tab;
DICT_COL*	g_col;
TRANS		g_tx;
int			g_tx_ct = 0;
int			g_ntx = 0;
int			g_nrec = 0;
SQL_PKG		g_pkg;		//解析后的sql事务链表

int			g_loop = 1;

int			g_dump_flag = 0;		//是否需要去重
char		g_dump_rsid_ssn[256];

char*		g_continue_sql = NULL;


static int conn_db(XOCI_CTX* ctx, XOCI_CONN* conn, const char* ip, int port, const char* svc, const char* user, const char* pwd)
{
	char	conn_str[1024];

	sprintf(conn_str, "(DESCRIPTION=(ADDRESS=(PROTOCOL=tcp)(HOST=%s)(PORT=%d))(CONNECT_DATA=(SERVICE_NAME=%s)))", ip, port, svc);

	if (xoci_init_context(ctx, conn_str, XOCI_INIT_DEFAULT) < 0)
	{
		LOG_E("xoci_init_context error[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (xoci_get_connection(ctx, conn, user, pwd) < 0)
	{
		LOG_E("xoci_get_connection error[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}
	return 0;
}

static void disconn_db(XOCI_CTX* ctx, XOCI_CONN* conn)
{
	if (conn)
	{
		xoci_close_connection(ctx, conn);
	}
	if (ctx)
	{
		xoci_free_context(ctx);
	}
}

static int get_cur_scn(XOCI_CTX* ctx, uint64_t* scn)
{
	char			sql[1024] = { 0 };
	XOCI_STMT		ostmt, *stmt;
	int				rbuf_sz = 1024;
	unsigned char*	rbuf;
	XOCI_DEFINE_SET	ds;
	XOCI_DC			dc[1];

	int				ret;
	int				rr1;

	stmt = &ostmt;
	memset(stmt, 0, sizeof(XOCI_STMT));
	sprintf(sql, "select to_char(current_scn) from v$database");

	if (xoci_prepare_statement(ctx, stmt, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03; dc[0].dtyp = XOCI_STR; dc[0].vsz = 32;
	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error");
		xoci_close_statement(ctx, stmt);
		return -1;
	}
	ds.nc = 1;
	ds.buf_sz = rbuf_sz;
	ds.buf = rbuf;
	ds.dc = dc;

	if (xoci_define(ctx, stmt, &ds) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_define err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (xoci_execute(ctx, stmt, 0) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_execute err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if ((ret = xoci_fetch(ctx, stmt, 1, &rr1)) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_fetch err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (ret == XOCI_NO_DATA)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("not found data");
		return -1;
	}

	if (dc[0].indp < 0)
	{
		*scn = 0;
	}
	else
	{
		*scn = atoll((char*)dc[0].valp);
	}

	Mem_Free(rbuf);
	xoci_close_statement(ctx, stmt);
	return 0;
}

static int get_cur_seq(XOCI_CTX* ctx, uint64_t scn, uint64_t* seq)
{
	char			sql[1024] = { 0 };
	XOCI_STMT		ostmt, *stmt;
	int				rbuf_sz = 1024;
	unsigned char*	rbuf;
	XOCI_DEFINE_SET	ds;
	XOCI_DC			dc[1];

	int				ret;
	int				rr1;

	stmt = &ostmt;
	memset(stmt, 0, sizeof(XOCI_STMT));
	sprintf(sql, "select max(sequence#) from v$log where first_change#<%"PRIu64, scn);

	if (xoci_prepare_statement(ctx, stmt, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03; dc[0].dtyp = XOCI_STR; dc[0].vsz = 128;
	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error");
		xoci_close_statement(ctx, stmt);
		return -1;
	}
	ds.nc = 1;
	ds.buf_sz = rbuf_sz;
	ds.buf = rbuf;
	ds.dc = dc;

	if (xoci_define(ctx, stmt, &ds) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_define err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (xoci_execute(ctx, stmt, 0) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_execute err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if ((ret = xoci_fetch(ctx, stmt, 1, &rr1)) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_fetch err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (ret == XOCI_NO_DATA)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("not found data");
		return -1;
	}

	
	*seq = atoll((char*)dc[0].valp);
	
	Mem_Free(rbuf);
	xoci_close_statement(ctx, stmt);
	return 0;
}

static int get_cur_logfile(XOCI_CTX* ctx, uint64_t seq, char* logfile)
{
	char			sql[1024] = { 0 };
	XOCI_STMT		ostmt, *stmt;
	int				rbuf_sz = 1024;
	unsigned char*	rbuf;
	XOCI_DEFINE_SET	ds;
	XOCI_DC			dc[1];

	int				ret;
	int				rr1;

	stmt = &ostmt;
	memset(stmt, 0, sizeof(XOCI_STMT));
	sprintf(sql, "select f.member from v$log l,v$logfile f where l.group#=f.group# and l.sequence#=%"PRIu64, seq);

	if (xoci_prepare_statement(ctx, stmt, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03; dc[0].dtyp = XOCI_STR; dc[0].vsz = 256;
	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error");
		xoci_close_statement(ctx, stmt);
		return -1;
	}
	ds.nc = 1;
	ds.buf_sz = rbuf_sz;
	ds.buf = rbuf;
	ds.dc = dc;

	if (xoci_define(ctx, stmt, &ds) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_define err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (xoci_execute(ctx, stmt, 0) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_execute err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if ((ret = xoci_fetch(ctx, stmt, 1, &rr1)) < 0)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("xoci_fetch err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (ret == XOCI_NO_DATA)
	{
		xoci_close_statement(ctx, stmt);
		Mem_Free(rbuf);
		LOG_E("not found data");
		return -1;
	}


	strcpy(logfile, (char*)dc[0].valp);

	Mem_Free(rbuf);
	xoci_close_statement(ctx, stmt);
	return 0;
}

static int filter_logcontent(LC_INFO* info)
{
	uint32_t	objn = 0;
	char*		p = NULL;

	if ((info->opc == 0) || (info->opc == 25) || (info->opc == 255))
	{
		return -1;
	}

	if (info->opc == 5)
	{
		//处理DDL相关
		if (strncmp(info->info, "INTERNAL DDL", strlen("INTERNAL DDL")) == 0)
		{
			return -1;
		}

		if (odt_lookup_schema(info->owner) < 0)
		{
			return -1;
		}
		return 0;
	}

	objn = info->objn;
	if ((info->objn == 0) && (info->objnm_ind != -1) && (info->opc != 6) && (info->opc != 7))
	{
		p = strchr(info->objname, '#');
		if (p == NULL)
		{
			return -1;
		}
		objn = atoi(++p);
	}

	if (objn > 0)
	{
		if (odt_check_obj(objn) < 0)
		{
			return -1;
		}
	}

	return 0;
}

//在这个函数中 如果没有加入链表的info 都需要释放 info->sql_redo 否则会造成内存泄漏
static int record_trans(LC_INFO* info)
{
	int		ret;

	switch(info->opc)
	{
	case 6:			//trans start
		ret = tx_insert(info->xid, info->scn);
		Mem_Free(info->sql_redo);
		if (ret < 0)
		{
			LOG_E("insert tx xid=[%"PRIu64"]", info->xid);
			return -1;
		}
		break;
	case 7:			//trans commit
		ret = tx_add(info, 1);
		if (ret < 0)
		{
			LOG_E("add tx commit xid=[%"PRIu64"]", info->xid);
			return -1;
		}
		break;
	case 36:		//trans rollback
		ret = tx_remove(info->xid);
		Mem_Free(info->sql_redo);
		if (ret < 0)
		{
			LOG_E("remove tx commit xid=[%"PRIu64"]", info->xid);
			return -1;
		}
		break;
	default:
		ret = tx_add(info, 0);
		if (ret < 0)
		{
			LOG_E("add tx xid=[%"PRIu64"]", info->xid);
			return -1;
		}
	}
	return 0;
}

static int analysis_record(TRANS* t, LC_INFO* info, char** psql)
{

	LOG_D("scn[%"PRIu64"] [%s] xid[%"PRIu64"] owner[%s] objnm[%s] opc[%u] rsid[%s] ssn[%u] csf[%u] sql[%s]",
		info->scn, info->time, info->xid, info->owner, info->objname, info->opc, info->rsid, info->ssn, info->csf, info->sql_redo);

	switch (info->opc)
	{
	case 1:			//insert
		parse_sql_insert(info, psql);
		break;
	case 2:			//delete
		parse_sql_delete(info, psql);
		break;
	case 3:			//update
		parse_sql_update(info, psql);
		break;
	case 5:			//ddl
		break;
	case 9:			//LOB_SELECT
		break;
	case 10:		//LOB_WRITE
		break;
	case 11:		//LOB_TRIM
		break;
	case 28:
	case 29:		//LOB_EARSE
		break;
	}
	return 0;
}

static int write_tx_info(TRANS* t)
{
	FILE* fp = NULL;

	fp = fopen("oracle_sql.sql", "a");
	if (fp == NULL)
	{
		LOG_E("open file[%s] error");
		return -1;
	}

	fprintf(fp, "----------\nxid[%"PRIu64"] start_scn[%"PRIu64"] last_scn[%"PRIu64"]\n", t->xid, t->sscn, t->scn);

	fclose(fp);
	return 0;
}

static int write_tx_sql(char* sql)
{
	FILE* fp = NULL;

	fp = fopen("oracle_sql.sql", "a");
	if (fp == NULL)
	{
		LOG_E("open file[%s] error");
		return -1;
	}

	fprintf(fp, "%s\n", sql);

	fclose(fp);
	return 0;
}

static int analysis_trans(void)
{
	TRANS*		p = NULL;
	TRANS*		n = NULL;
	TRANS*		q = NULL;
	REC_INFO*	head = NULL;
	REC_INFO*	r = NULL;

	TRANS		temp;

	memset(&temp, 0, sizeof(TRANS));

	for (p = &g_tx; p->next;)
	{
		n = p->next;
		head = &n->node;

		if (n->flg == 1)			//该事务已经commit
		{
			write_tx_info(n);
			while (head->next != NULL)
			{
				r = head->next;
				head->next = r->next;
				if (n->ct > 1)			//事物已经提交，并且有有效数据
				{
					analysis_record(n, &r->info, &r->sql_anl);
					if (r->sql_anl)
					{
						write_tx_sql(r->sql_anl);
						Mem_Free(r->sql_anl);
					}
				}
				//如果没有有效数据，只有一个commit 则直接删除
				if (r->info.sql_redo) Mem_Free(r->info.sql_redo);
				Mem_Free(r);
				g_nrec--;
			}
			p->next = n->next;
			Mem_Free(n);
			g_ntx--;
			g_tx_ct--;
		}
		else
		{
			//事务还未提交
			p->next = n->next;
			for (q = &temp; q->next; q = q->next);
			{
				;
			}
			q->next = n;
			n->next = NULL;
		}
	}

	g_tx.next = temp.next;
	LOG_D("exist uncommit tx ct=[%d]...", g_tx_ct);
	tx_travel();
	return 0;
}

static int analysis_logcontent(XOCI_CTX* ctx, uint64_t* last_scn, char* last_dump_info, int dump_flag)
{
	int			i;
	char		sql[1024];
	XOCI_STMT	otmt, *stmtp;
	XOCI_DEFINE_SET ds;
	XOCI_DC		dc[24];
	char*		buf = NULL;
	int			buf_sz = 2 * 1024 * 1024;
	int			ret;
	int			rrows = 0;
	LC_INFO		info;
	int			find_dump_flag = 0;
	int			len = 0;
	char		temp_sql[4096] = { 0 };

	stmtp = &otmt;
	memset(stmtp, 0, sizeof(otmt));

	sprintf(sql, "select scn,to_char(TIMESTAMP, 'YYYYMMDD HH24MISS'),THREAD#,XIDUSN,XIDSLT,XIDSQN,RBASQN,RBABLK,RBABYTE,"
		"DATA_OBJ#,SEG_OWNER,SEG_NAME,SEG_TYPE,ROW_ID,SESSION#,SERIAL#,ROLLBACK,OPERATION_CODE,SQL_REDO,RS_ID,"
		"SEQUENCE#,SSN,CSF,INFO from v$logmnr_contents");

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	buf = Mem_Malloc(buf_sz);
	if (buf == NULL)
	{
		LOG_E("malloc error");
		xoci_close_statement(ctx, stmtp);
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_STR;	dc[0].vsz = 32;			//scn
	dc[1].flg = 0x03;	dc[1].dtyp = XOCI_STR;	dc[1].vsz = 16;			//timestamp
	dc[2].flg = 0x03;	dc[2].dtyp = XOCI_INT;	dc[2].vsz = 4;			//thread#
	dc[3].flg = 0x03;	dc[3].dtyp = XOCI_INT;	dc[3].vsz = 4;			//XIDUSN
	dc[4].flg = 0x03;	dc[4].dtyp = XOCI_INT;	dc[4].vsz = 4;			//XIDSLT
	dc[5].flg = 0x03;	dc[5].dtyp = XOCI_STR;	dc[5].vsz = 16;			//XIDSQN
	dc[6].flg = 0x03;	dc[6].dtyp = XOCI_INT;	dc[6].vsz = 4;			//RBASQN
	dc[7].flg = 0x03;	dc[7].dtyp = XOCI_STR;	dc[7].vsz = 16;			//RBABLK
	dc[8].flg = 0x03;	dc[8].dtyp = XOCI_INT;	dc[8].vsz = 4;			//RBABYTE
	dc[9].flg = 0x03;	dc[9].dtyp = XOCI_STR;	dc[9].vsz = 16;			//DATA_OBJ#
	dc[10].flg = 0x0B;	dc[10].dtyp = XOCI_STR;	dc[10].vsz = 32;		//SEG_OWNER
	dc[11].flg = 0x0B;	dc[11].dtyp = XOCI_STR;	dc[11].vsz = 32;		//SEG_NAME
	dc[12].flg = 0x03;	dc[12].dtyp = XOCI_INT;	dc[12].vsz = 4;			//SEG_TYPE
	dc[13].flg = 0x03;	dc[13].dtyp = XOCI_STR;	dc[13].vsz = 20;		//ROW_ID
	dc[14].flg = 0x03;	dc[14].dtyp = XOCI_INT;	dc[14].vsz = 4;			//SESSION#
	dc[15].flg = 0x03;	dc[15].dtyp = XOCI_INT;	dc[15].vsz = 4;			//SERIAL#
	dc[16].flg = 0x03;	dc[16].dtyp = XOCI_INT;	dc[16].vsz = 4;			//ROLLBACK
	dc[17].flg = 0x03;	dc[17].dtyp = XOCI_INT;	dc[17].vsz = 4;			//OPERATION_CODE
	dc[18].flg = 0x03;	dc[18].dtyp = XOCI_STR;	dc[18].vsz = 4096;		//SQL_REDO
	dc[19].flg = 0x03;	dc[19].dtyp = XOCI_STR;	dc[19].vsz = 64;		//RS_ID
	dc[20].flg = 0x03;	dc[20].dtyp = XOCI_STR;	dc[20].vsz = 16;		//SEQUENCE#
	dc[21].flg = 0x03;	dc[21].dtyp = XOCI_STR;	dc[21].vsz = 16;		//SSN
	dc[22].flg = 0x03;	dc[22].dtyp = XOCI_INT;	dc[22].vsz = 4;			//CSF
	dc[23].flg = 0x0B;	dc[23].dtyp = XOCI_STR;	dc[23].vsz = 32;		//INFO


	ds.nc = 24;
	ds.buf_sz = buf_sz;
	ds.buf = buf;
	ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		xoci_close_statement(ctx, stmtp);
		Mem_Free(buf);
		LOG_E("xoci_define err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		xoci_close_statement(ctx, stmtp);
		Mem_Free(buf);
		LOG_E("xoci_execute err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	while (g_loop)
	{
		memset(&info, 0, sizeof(info));
		ret = xoci_fetch(ctx, stmtp, ds.nrows, &rrows);
		if (ret < 0)
		{
			xoci_close_statement(ctx, stmtp);
			Mem_Free(buf);
			LOG_E("xoci_fetch err[%d-%s]", excp_get_code(), excp_get_message());
			return -1;
		}

		//LOG_D("fetch [%d] rows...", rrows);
		for (i = 0; i < rrows; i++)
		{
			info.scn = atoll((char*)dc[0].valp + i * 32);
			strcpy(info.time, (char*)((char*)dc[1].valp + i * 16));
			info.thread = *((uint32_t*)dc[2].valp + i);
			info.xusn = *((uint32_t*)dc[3].valp + i);
			info.xslt = *((uint32_t*)dc[4].valp + i);
			info.xsqn = atoll((char*)dc[5].valp + i * 16);
			info.xid = ((info.xusn << 48) | (info.xslt << 32) | info.xsqn);
			info.rbasqn = *((uint32_t*)dc[6].valp + i);
			info.rbablk = atoll((char*)dc[7].valp + i * 16);
			info.rbabyte = *((uint32_t*)dc[8].valp + i);
			info.objn = atoi((char*)dc[9].valp + i * 16);
			strcpy(info.owner, (char*)((char*)dc[10].valp + i * 32));
			strcpy(info.objname, (char*)((char*)dc[11].valp + i * 32));
			info.objnm_ind = *(dc[11].indp + i);
			info.seg_type = *((uint32_t*)dc[12].valp + i);
			strcpy(info.rowid, (char*)((char*)dc[13].valp + i * 20));
			info.rollback = *((uint32_t*)dc[16].valp + i);
			info.opc = *((uint32_t*)dc[17].valp + i);
			//strcpy(info.sql_redo, (char*)((char*)dc[18].valp + i * 4096));
			strcpy(temp_sql, (char*)((char*)dc[18].valp + i * 4096));
			strcpy(info.rsid, (char*)((char*)dc[19].valp + i * 64));
			info.seq = atoll((char*)dc[20].valp + i * 16);
			info.ssn = atoi((char*)dc[21].valp + i * 16);
			info.csf = *((uint32_t*)dc[22].valp + i);
			strcpy(info.info, (char*)((char*)dc[23].valp + i * 32));


			*last_scn = info.scn;
			sprintf(last_dump_info, "%s_%u_%u_%d", info.rsid, info.ssn, info.objn, info.opc);
			if ((find_dump_flag == 0) && (dump_flag != 0))	//还没找到并且重复标志为1 就需要找上次分析的点
			{
				if (strcmp(last_dump_info, g_dump_rsid_ssn) != 0)
				{
					continue;
				}
				find_dump_flag = 1;
				//LOG_D("find last analysis point [%s]", last_dump_info);
				continue;		//本次找到的这个点也需要跳过
			}

			//过滤消息
			if (filter_logcontent(&info) < 0)
			{
				continue;
			}

			//如果是csf 表示未完成，则记录当前sql 并舍弃该条
			if (info.csf == 1)
			{
				if (g_continue_sql == NULL)
				{
					g_continue_sql = (char*)Mem_Malloc(strlen(temp_sql) + 1);
					if (g_continue_sql == NULL)
					{
						LOG_E("malloc g_continue_sql sql error");
						Mem_Free(buf);
						xoci_close_statement(ctx, stmtp);
						return -1;
					}
					memset(g_continue_sql, 0, strlen(temp_sql) + 1);
				}
				else
				{
					len = strlen(g_continue_sql) + 1 + strlen(temp_sql);
					g_continue_sql = (char*)Mem_Realloc(g_continue_sql, len);
					if (g_continue_sql == NULL)
					{
						LOG_E("realloc g_continue_sql sql error");
						Mem_Free(buf);
						xoci_close_statement(ctx, stmtp);
						return -1;
					}
				}
				strcat(g_continue_sql, temp_sql);
				continue;
			}

			//记录sql 如果之前有值 则用之前的值
			len = (g_continue_sql != NULL) ? (strlen(g_continue_sql) +strlen(temp_sql)+ 1) : (strlen(temp_sql) + 1);
			info.sql_redo = (char*)Mem_Malloc(len);
			if (info.sql_redo == NULL)
			{
				LOG_E("malloc info.sql_redo sql error");
				Mem_Free(buf);
				xoci_close_statement(ctx, stmtp);
				return -1;
			}
			if (g_continue_sql != NULL)
			{
				snprintf(info.sql_redo, len, "%s%s", g_continue_sql, temp_sql);
				Mem_Free(g_continue_sql);
				g_continue_sql = NULL;
			}
			else
			{
				strcpy(info.sql_redo, temp_sql);
			}
			

			/*LOG_D("scn[%"PRIu64"] [%s] xid[%"PRIu64"] owner[%s] objnm[%s] opc[%u] rsid[%s] ssn[%u] csf[%u] sql[%s]", 
				info.scn, info.time, info.xid, info.owner, info.objname, info.opc, info.rsid, info.ssn, info.csf, info.sql_redo);
			*/
			//根据事物记录各个数据
			if (record_trans(&info) < 0)
			{
				continue;
			}
		}

		if (ret == XOCI_NO_DATA)
		{
			break;
		}
	}

	//重复标志位1 但是没有找到
	if ((dump_flag == 1) && (find_dump_flag == 0))
	{
		LOG_E("not found last dump record");
	}
	Mem_Free(buf);
	xoci_close_statement(ctx, stmtp);
	return 0;
}

static void run(XOCI_CTX* ctx)
{
	static uint64_t	start_scn = 0;
	static uint64_t	end_scn = 0;
	uint64_t		seq = 0;
	uint64_t		seq_end = 0;
	char			logfile[1024] = { 0 };
	char			logfile_end[1024] = { 0 };
	char			dump_tmp[128] = { 0 };

	if (start_scn == 0)
	{
		if (g_start_scn != 0)
		{
			start_scn = g_start_scn;
		}
		else
		{
			LOG_D("get start scn...");
			if (get_cur_scn(ctx, &start_scn) < 0)
			{
				LOG_E("get cur scn error");
				return;
			}
		}
	}
	else
	{
		//切换日志
		lmr_remove(ctx);
	}

	if (get_cur_seq(ctx, start_scn, &seq) < 0)
	{
		LOG_E("get_cur_seq error");
		return ;
	}

	if (get_cur_logfile(ctx, seq, logfile) < 0)
	{
		LOG_E("get_cur_logfile error");
		return ;
	}

	if (lmr_setlog(ctx, logfile, 0) < 0)
	{
		LOG_E("lmr_setlog error");
		return ;
	}

	if (end_scn == 0 && g_end_scn != 0)
	{
		end_scn = g_end_scn;
	}
	else
	{
		if (get_cur_scn(ctx, &end_scn) < 0)
		{
			LOG_E("get_cur_scn end error");
			return ;
		}

		if (get_cur_seq(ctx, end_scn, &seq_end) < 0)
		{
			LOG_E("get_cur_seq error");
			return ;
		}

		if (seq != seq_end)
		{
			//这里如果 endscn 已经是另外一个seq 则需要将logfile加入到logminer进行分析
			if (get_cur_logfile(ctx, seq_end, logfile_end) < 0)
			{
				LOG_E("get_cur_logfile error");
				return ;
			}
			if (lmr_setlog(ctx, logfile_end, 1) < 0)
			{
				LOG_E("lmr_setlog error");
				return ;
			}
		}
	}

	LOG_D("start_scn=[%"PRIu64"] end_scn=[%"PRIu64"] cur_log_seq=[%"PRIu64"] cur_logfile=[%s] end_log_seq=[%"PRIu64"] end_logfile=[%s]",
			start_scn, end_scn, seq, logfile, seq_end, logfile_end);

	if (lmr_start(ctx, start_scn, end_scn) < 0)
	{
		LOG_E("lmr_start error");
		return ;
	}

	if (analysis_logcontent(ctx, &start_scn, dump_tmp, g_dump_flag) < 0)
	{
		LOG_E("analysis_logcontent error");
		return ;
	}

	//LOG_D("malloc tx_ct[%d] record_ct[%d]", g_ntx, g_nrec);

	if (analysis_trans() < 0)
	{
		LOG_E("analysis_trans error");
		return;
	}

	strcpy(g_dump_rsid_ssn, dump_tmp);
	if ((g_dump_flag == 0) && (g_dump_rsid_ssn[0] != '\0'))
	{
		g_dump_flag = 1;
	}

	//LOG_D("last rsid_ssn is[%s]", g_dump_rsid_ssn);

	if (lmr_end(ctx) < 0)
	{
		LOG_E("lmr_end error");
		return;
	}
}

static void test_cap(const char* ip, int port, const char* svc, const char* user, const char* pwd)
{
	XOCI_CTX		octx, *ctx;
	XOCI_CONN		oconn, *conn;

	ctx = &octx;
	conn = &oconn;

	memset(ctx, 0, sizeof(octx));
	memset(conn, 0, sizeof(oconn));

	if (conn_db(ctx, conn, ip, port, svc, user, pwd) < 0)
	{
		LOG_E("conn_db error");
		return;
	}
	LOG_D("connection db success...");

	LOG_D("get dict...");
	get_dict(ctx, "KYLIN");
	get_dict_pk(ctx, "KYLIN");
	get_UDT_dict_info(ctx);

	show_dict();
	//g_loop = 0;
	while (g_loop)
	{
		run(ctx);
		LOG_D("wait %d seconds", 5);
		sleep(5);
	}

	if (g_continue_sql) Mem_Free(g_continue_sql);
	free_tx();
	disconn_db(ctx, conn);
}


int main(int argc, char* argv[])
{
	LOG_Init(LOG_LEV_DEBUG, "testcap", "testcap");
	Mem_SetDbgOn();
	init_signal();

	if (argc == 2) 
	{
		g_start_scn = atoll(argv[1]);
	}
	if (argc == 3)
	{
		g_start_scn = atoll(argv[1]);
		g_end_scn = atoll(argv[2]);
	}

	LOG_D("argc ---> start_scn=["PRIu64"] end_scn=["PRIu64"]", g_start_scn, g_end_scn);

	g_dict = (DICT_LIST*)Mem_Malloc(sizeof(DICT_LIST) * MAX_SCHEMA_NUM);
	if (g_dict == NULL)
	{
		LOG_E("malloc DICT_LIST error[%s]", strerror(errno));
		goto err;
	}
	g_tab = (DICT_TAB*)Mem_Malloc(sizeof(DICT_TAB) * MAX_TAB_NUM);
	if (g_tab == NULL)
	{
		LOG_E("malloc DICT_TAB error[%s]", strerror(errno));
		goto err;
	}
	g_col = (DICT_COL*)Mem_Malloc(sizeof(DICT_COL) * MAX_COL_NUM);
	if (g_col == NULL)
	{
		LOG_E("malloc DICT_COL error[%s]", strerror(errno));
		goto err;
	}
	memset(g_dict, 0, sizeof(DICT_LIST) * MAX_SCHEMA_NUM);
	memset(g_tab, 0, sizeof(DICT_TAB) * MAX_TAB_NUM);
	memset(g_col, 0, sizeof(DICT_COL) * MAX_COL_NUM);
	memset(g_dump_rsid_ssn, 0, sizeof(g_dump_rsid_ssn));
	memset(&g_tx, 0, sizeof(TRANS));
	memset(&g_pkg, 0, sizeof(SQL_PKG));

	test_cap("172.16.1.198", 1521, "orcl", "kylin", "oracle");

err:
	if (g_dict) Mem_Free(g_dict);
	if (g_tab) Mem_Free(g_tab);
	if (g_col) Mem_Free(g_col);
	Mem_Leak();
	return 0;
}


