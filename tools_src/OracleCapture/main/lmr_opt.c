#include "basetype.h"
#include "testcap.h"
#include "lmr_opt.h"


static int cf_exec_sql(XOCI_CTX* ctx, char* sql)
{
	XOCI_STMT	ostmt, *stmtp;

	stmtp = &ostmt;
	memset(stmtp, 0, sizeof(ostmt));

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err,sql=[%s] [%d-%s]", sql, excp_get_code(), excp_get_message());
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 1) < 0)
	{
		xoci_close_statement(ctx, stmtp);
		LOG_E("xoci_execute err [%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	xoci_close_statement(ctx, stmtp);
	return 0;
}

int lmr_setlog(XOCI_CTX* ctx, char* logfile, int flag)
{
	char	sql[1024];
	char	opt[1024];

	if (flag == 0)
	{
		sprintf(opt, "NEW");
	}
	else
	{
		sprintf(opt, "ADDFILE");
	}

	sprintf(sql, "BEGIN SYS.DBMS_LOGMNR.ADD_LOGFILE('%s', SYS.DBMS_LOGMNR.%s); END; ", logfile, opt);

	if (cf_exec_sql(ctx, sql) < 0)
	{
		LOG_E("logmnr add_logfile error");
		return -1;
	}

	return 0;

}

int lmr_start(XOCI_CTX* ctx, uint64_t start_scn, uint64_t end_scn)
{
	char	sql[1024];

	sprintf(sql, "BEGIN SYS.DBMS_LOGMNR.START_LOGMNR(STARTSCN=>%"PRIu64", ENDSCN=>%"PRIu64", OPTIONS=>SYS.DBMS_LOGMNR.SKIP_CORRUPTION); END; ", 
		start_scn, end_scn);

	if (cf_exec_sql(ctx, sql) < 0)
	{
		LOG_E("logmnr start error");
		return -1;
	}

	return 0;
}

int lmr_end(XOCI_CTX* ctx)
{
	char	sql[1024];

	sprintf(sql, "BEGIN SYS.DBMS_LOGMNR.END_LOGMNR; END; ");

	if (cf_exec_sql(ctx, sql) < 0)
	{
		LOG_E("logmnr end error");
		return -1;
	}

	return 0;
}

int lmr_remove(XOCI_CTX* ctx)
{
	int			ret;
	int			rrows = 0;
	char		sql[1024] = { 0 };
	XOCI_STMT	ostmt, *stmtp;
	XOCI_DEFINE_SET ds;
	XOCI_DC		dc[4];
	char		buf[8092];
	char*		logfile;
	uint32_t	seq;
	char*		low_scn;
	char*		next_scn;

	stmtp = &ostmt;
	memset(stmtp, 0, sizeof(ostmt));

	sprintf(sql, "select FILENAME,THREAD_SQN,LOW_SCN,NEXT_SCN from v$logmnr_logs");

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err,sql=[%s] [%d-%s]", sql, excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_STR;	dc[0].vsz = 520;
	dc[1].flg = 0x03;	dc[1].dtyp = XOCI_INT;	dc[1].vsz = 4;
	dc[2].flg = 0x03;	dc[2].dtyp = XOCI_STR;	dc[2].vsz = 16;
	dc[3].flg = 0x03;	dc[3].dtyp = XOCI_STR;	dc[3].vsz = 16;

	ds.nc = 4; ds.buf_sz = 8092; ds.buf = buf; ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		xoci_close_statement(ctx, stmtp);
		LOG_E("xoci_define error[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		xoci_close_statement(ctx, stmtp);
		LOG_E("xoci_execute err [%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	while (1)
	{
		if ((ret = xoci_fetch(ctx, stmtp, 1, &rrows)) < 0)
		{
			xoci_close_statement(ctx, stmtp);
			LOG_E("xoci_fetch err [%d-%s]", excp_get_code(), excp_get_message());
			return -1;
		}

		if (ret == XOCI_NO_DATA)
		{
			break;
		}

		logfile = (char*)dc[0].valp;
		seq = *((uint32_t *)dc[1].valp);
		low_scn = (char*)dc[2].valp;
		next_scn = (char*)dc[3].valp;

		LOG_D("exist log info [%06d][%s][%d-%s]", seq, logfile, low_scn, next_scn);

		sprintf(sql, "BEGIN SYS.DBMS_LOGMNR.REMOVE_LOGFILE('%s'); END;", logfile);

		if (cf_exec_sql(ctx, sql) < 0)
		{
			xoci_close_statement(ctx, stmtp);
			LOG_E("logmnr end error");
			return -1;
		}
	}

	xoci_close_statement(ctx, stmtp);
	return 0;
}



