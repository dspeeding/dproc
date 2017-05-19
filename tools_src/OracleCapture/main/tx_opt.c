#include "basetype.h"
#include "testcap.h"
#include "tx_opt.h"

extern TRANS	g_tx;
extern int		g_tx_ct;
extern int		g_ntx;
extern int		g_nrec;

int tx_insert(uint64_t xid, uint64_t scn)
{
	TRANS*	p = NULL;
	TRANS*	q = NULL;

	p = (TRANS*)Mem_Malloc(sizeof(TRANS));
	if (p == NULL)
	{
		LOG_E("malloc trans error");
		return -1;
	}
	g_ntx++;

	memset(p, 0, sizeof(TRANS));

	p->xid = xid;
	p->scn = scn;
	p->sscn = scn;
	p->ct = 0;
	p->next = NULL;

	//定位到最后
	for (q = &g_tx; q->next; q = q->next)
	{
		;
	}

	q->next = p;
	g_tx_ct++;

	return 0;
}

int tx_remove(uint64_t xid)
{
	TRANS*		p = NULL;
	TRANS*		n = NULL;
	REC_INFO*	head = NULL;
	REC_INFO*	r = NULL;
	int			find = 0;

	for (p = &g_tx; p->next; p = p->next)
	{
		n = p->next;
		if (n->xid == xid)
		{
			find = 1;
			head = &n->node;

			while (head->next != NULL)
			{
				r = head->next;
				head->next = r->next;
				if (r->info.sql_redo != NULL) Mem_Free(r->info.sql_redo);
				Mem_Free(r);
				g_nrec--;
			}

			p->next = n->next;
			Mem_Free(n);
			g_ntx--;
			g_tx_ct--;
			break;
		}
	}

	if (find == 0)
	{
		LOG_W("not find tx rollback xid[%"PRIu64"]", xid);
		return 0;
	}
	return 0;
}

//这里 commit_fiag 0表示正常模式 commit_flag 1表示commit模式
int tx_add(LC_INFO* info, int commit_flag)
{
	TRANS*		p = NULL;
	REC_INFO*	head = NULL;
	REC_INFO*	r = NULL;
	REC_INFO*	q = NULL;
	int			find = 0;

	for (p = g_tx.next; p; p = p->next)
	{
		if (p->xid == info->xid)
		{
			find = 1;
			break;
		}
	}

	if(find == 0)	
	{ 
		Mem_Free(info->sql_redo);
		if (commit_flag == 1)
		{
			LOG_W("not find tx commit xid{%"PRIu64"]", info->xid);
			return 0;
		}
		else
		{
			LOG_E("not find tx commit xid{%"PRIu64"]", info->xid);
			return -1;
		}
	}

	head = &p->node;
	r = (REC_INFO*)Mem_Malloc(sizeof(REC_INFO));
	if (r == NULL)
	{
		Mem_Free(info->sql_redo);
		LOG_E("malloc rec_info error");
		return -1;
	}

	g_nrec++;
	memset(r, 0, sizeof(REC_INFO));
	memcpy(&r->info, info, sizeof(LC_INFO));			//这里发生了浅拷贝  不要在释放info.sql_redo
	r->next = NULL;

	//定位到最后
	for (q = head; q->next; q = q->next)
	{
		;
	}

	q->next = r;
	p->ct++;
	p->scn = info->scn;

	if (commit_flag == 1)
	{
		p->flg = 1;			//表示已经完成事务
		if (p->ct == 1)		//如果已经commit的事务只有commit一条， 则删除该事务
		{
			tx_remove(p->xid);
		}
	}
	return 0;
}

void tx_travel(void)
{
	TRANS*		p;
//	REC_INFO*	head = NULL;
//	REC_INFO*	r = NULL;

	LOG_D("tx --- total[%d]...", g_tx_ct);

	for (p = g_tx.next; p; p = p->next)
	{
		LOG_D("tx flag[%d] ct[%d] scn[%"PRIu64"] xid[%"PRIu64"] sscn[%"PRIu64"]", 
			p->flg, p->ct, p->scn, p->xid, p->sscn);

		LOG_D("xid[%"PRIu64"]--------------------->", p->xid);

// 		head = &p->node;
// 		for (r = head->next; r; r = r->next)
// 		{
// 			LOG_D("scn[%"PRIu64"] [%s] xid[%"PRIu64"] owner[%s] objnm[%s] opc[%u] rsid[%s] ssn[%u] csf[%u] sql[%s]",
// 				r->info.scn, r->info.time, r->info.xid, r->info.owner, r->info.objname, r->info.opc, r->info.rsid, 
// 				r->info.ssn, r->info.csf, r->info.sql_redo);
// 		}
		
		LOG_D("xid[%"PRIu64"]---------------------<", p->xid);
	}
}

void free_tx(void)
{
	TRANS*		p = NULL;
	TRANS*		n = NULL;
	REC_INFO*	head = NULL;
	REC_INFO*	r = NULL;

	for (p = &g_tx; p->next;)
	{
		n = p->next;
		head = &n->node;

		while (head->next != NULL)
		{
			r = head->next;
			head->next = r->next;
			Mem_Free(r);
			g_nrec--;
		}

		p->next = n->next;
		Mem_Free(n);
		g_ntx--;
		g_tx_ct--;
	}

	LOG_D("tx free g_ntx=[%d] g_nrec = [%d]", g_ntx, g_nrec);
}
