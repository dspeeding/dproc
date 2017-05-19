#include "basetype.h"
#include "testcap.h"
#include "parse_sql.h"
#include "dict_opt.h"
#include "type_transfer.h"

#define MAX_SQL_LEN		2*1024*1024

extern SQL_PKG		g_pkg;
extern DICT_COL*	g_col;

int					g_top_null_index = -1;

int					g_old_top_null_index = -1;

static int get_column_owner_tab(char* pc, uint32_t objn, DICT_COL** pcol)
{
	int			scid;
	char*		q;

	for (; *pc && ((*pc == ' ') || (*pc == '\t')); pc++);

	if (strncmp(pc, "\"COL", 4) != 0)
	{
		LOG_E("sql error not find COL");
		return -1;
	}

	pc += 4;
	for (; *pc && ((*pc == ' ') || (*pc == '\t')); pc++);

	q = strchr(pc, '\"');
	if (q == NULL)
	{
		LOG_E("sql error ");
		return -1;
	}
	*q = '\0';

	scid = atoi(pc);

	if (odt_lookup_colobj(objn, scid, pcol) < 0)
	{
		LOG_E("look colobj[%d] not find in dict", scid);
		return -1;
	}

	return 0;
}

static int get_column_value(SQL_VECTOR*	r, char* val)
{
	if ((r->flag & VECTOR_FLAG_NULL) == VECTOR_FLAG_NULL)
	{
		strcpy(val, "NULL");
	}
	else if ((r->flag & VECTOR_FLAG_NOT_NULL) == VECTOR_FLAG_NOT_NULL)
	{
		strcpy(val, "NOT NULL");
	}
	else if ((r->flag & VECTOR_FLAG_DYNAMIC) == VECTOR_FLAG_DYNAMIC)		//TODO
	{
		strcpy(val, "NULL");
	}
	else
	{
		if (get_column_type_value(r->ctype, r->cdata, val, r->pcol) < 0)
		{
			LOG_E("get_column_type_value error");
			return -1;
		}
	}
	return 0;
}

//�ݹ��ȡ�е���������
static void get_dict_column_typename_start(DICT_COL* col, char* name, UDT_NULL_INFO* uni)
{
	int			i;
	char		temp[256] = { 0 };
	DICT_COL*	pcol;
	int			last_index = -1;
	int			find = 0;

	pcol = col;

	//LOG_D("parse col start, index=[%d] name=[%s]", col->self_index, col->name);

	while ((pcol->p_col_index != -1) && (pcol->sub_index == 1))
	{
		sprintf(temp, "%s(%s", g_col[pcol->p_col_index].type_name, name);
		strcpy(name, temp);

		for (i = 1; i < MAX_UDT_NULL_NUM; i++)
		{
			if (uni[i].used == 0)
			{
				break;
			}
			if (uni[i].col_index != pcol->p_col_index)
			{
				continue;
			}
			//
			if (uni[i].is_null == 1)
			{
				//LOG_D("get col's parent is null, index=[%d] name=[%s] pindex=[%d], pname=[%s]", pcol->self_index, pcol->name, g_col[pcol->p_col_index].self_index, g_col[pcol->p_col_index].name);
				last_index = pcol->p_col_index;
				strcpy(name, "NULL,");
			}
			break;
		}

		pcol = &g_col[pcol->p_col_index];
	}

	//LOG_D("it's most top null index is[%d]", last_index);
	if (last_index == -1)
	{
		g_top_null_index = -1;
	}
	else
	{
		if (g_top_null_index == -1)
		{
			g_top_null_index = last_index;
		}
		else
		{
			//�����NULL�ģ����丸���Ƿ���NULL��
			find = 0;
			pcol = &g_col[last_index];
			while (pcol->p_col_index != -1)
			{
				if (pcol->p_col_index == g_top_null_index)
				{
					LOG_D("col's null ommit index=[%d] name=[%s]", col->self_index, col->name);
					strcpy(name, "");
					find = 1;
					break;
				}
				pcol = &g_col[pcol->p_col_index];
			}
			if (find == 0)
			{
				//��ʾ�����null�и�֮ǰ���еĲ㼶�߻�����ͬ
				g_top_null_index = last_index;
			}
		}
	}
}

static void get_dict_column_typename_end(DICT_COL* col, char* name, UDT_NULL_INFO* uni)
{
	int			i;
	DICT_COL*	pcol;

	pcol = col;

	while ((pcol->p_col_index != -1) && (pcol->sub_index == g_col[pcol->p_col_index].parm_num))
	{
		strcat(name, ")");
		for (i = 1; i < MAX_UDT_NULL_NUM; i++)
		{
			if (uni[i].used == 0)
			{
				break;
			}
			if (uni[i].col_index != pcol->p_col_index)
			{
				continue;
			}
			//
			if (uni[i].is_null == 1)
			{
				strcpy(name, "");
			}
			break;
		}
		pcol = &g_col[pcol->p_col_index];
	}
}

static void get_dict_column_typename_start_limit(DICT_COL* col, char* name, int end_index)
{
	char		temp[256] = { 0 };
	DICT_COL*	pcol;

	pcol = col;
	while ((pcol->p_col_index != -1) && (pcol->sub_index == 1))
	{
		sprintf(temp, "%s(%s", g_col[pcol->p_col_index].type_name, name);
		strcpy(name, temp);

		if (pcol->p_col_index == end_index)
		{
			break;
		}

		pcol = &g_col[pcol->p_col_index];
	}
}

//���� start_index �Ĳ㼶����С�� end_index �ļ���
static void get_dict_column_typename_start_end_limit(DICT_COL* col, char* name, int start_index, int end_index)
{
	char		temp[256] = { 0 };
	DICT_COL*	pcol;

	pcol = &g_col[start_index];
	while (pcol->p_col_index != -1)
	{
		sprintf(temp, "%s(%s", g_col[pcol->p_col_index].type_name, name);
		strcpy(name, temp);

		if (pcol->p_col_index == end_index)
		{
			break;
		}

		pcol = &g_col[pcol->p_col_index];
	}
}

static void get_dict_column_typename_end_start_limit(DICT_COL* col, char* name, int end_index)
{
	DICT_COL*	pcol;

	pcol = col;

	while ((pcol->p_col_index != -1) && (pcol->sub_index == g_col[pcol->p_col_index].parm_num))
	{
		strcat(name, ")");

		if (pcol->p_col_index == end_index)
		{
			break;
		}
		pcol = &g_col[pcol->p_col_index];
	}
}

//���� start_index �Ĳ㼶����С�� end_index �ļ���
static void get_dict_column_typename_end_start_end_limit(DICT_COL* col, char* name, int start_index, int end_index)
{
	DICT_COL*	pcol;
	int			find = 0;

	pcol = col;

	while ((pcol->p_col_index != -1) && (pcol->sub_index == g_col[pcol->p_col_index].parm_num))
	{
		if ((pcol->p_col_index != start_index) && (find == 0))
		{
			pcol = &g_col[pcol->p_col_index];
			continue;
		}
		if (pcol->p_col_index == start_index)
		{
			pcol = &g_col[pcol->p_col_index];
			find = 1;
			continue;
		}

		strcat(name, ")");

		if (pcol->p_col_index == end_index)
		{
			break;
		}
		pcol = &g_col[pcol->p_col_index];
	}
}

static void get_topmost_null_index(DICT_COL* col, int* ptopmost_index, int* plevel, UDT_NULL_INFO* uni)
{
	int			i;
	DICT_COL*	pcol;
	int			level = 0;

	pcol = col;

	while (pcol->p_col_index != -1)
	{
		for (i = 1; i < MAX_UDT_NULL_NUM; i++)
		{
			if (uni[i].used == 0)
			{
				break;
			}
			if (uni[i].col_index != pcol->p_col_index)
			{
				continue;
			}
			//
			if (uni[i].is_null == 1)
			{
				*ptopmost_index = pcol->p_col_index;
				*plevel = level;
			}
			break;
		}

		level++;
		pcol = &g_col[pcol->p_col_index];
	}
}

static void get_column_full_name(int cur_index, char* name)
{
	DICT_COL*	pcol;
	char		temp[256] = { 0 };

	pcol = &g_col[cur_index];
	strcpy(name, pcol->name);

	while (pcol->p_col_index != -1)
	{
		sprintf(temp, "%s.%s", g_col[pcol->p_col_index].name, name);
		strcpy(name, temp);

		pcol = &g_col[pcol->p_col_index];
	}
}

static int parse_column(char* sql, char* pc, char* pv, uint32_t objn, SQL_VECTOR* vhead, DICT_COL** pcol)
{
	char*		q;
	SQL_VECTOR*	r;
	SQL_VECTOR*	vt;
	DICT_COL*	col = NULL;

	//��ʼ��������Ϣ
	if (get_column_owner_tab(pc, objn, pcol) < 0)
	{
		LOG_E("get_column_owner_tab error");
		return -1;
	}
	col = *pcol;

	vt = (SQL_VECTOR*)Mem_Malloc(sizeof(SQL_VECTOR));
	if (vt == NULL)
	{
		LOG_E("malloc SQL_VECTOR error");
		return -1;
	}
	memset(vt, 0, sizeof(SQL_VECTOR));

	vt->ctype = col->type;
	vt->pcol = col;
	strcpy(vt->cname, col->name);
	//LOG_D("get type[%d] name[%s]", vt->ctype, vt->cname);

	for (; *pv && ((*pv == ' ') || (*pv == '\t')); pv++);

	//��ʼ����ֵ��Ϣ
	if (strncmp(pv, "NULL", 4) == 0)
	{
		vt->flag |= VECTOR_FLAG_NULL;
	}
	else if (strncmp(pv, "NOT NULL", 8) == 0)
	{
		vt->flag |= VECTOR_FLAG_NOT_NULL;
	}
	else if (strncmp(pv, "LONG DATA", 10) == 0)
	{
		vt->flag |= VECTOR_FLAG_DYNAMIC;
	}
	else if (strncmp(pv, "HEXTORAW", 8) == 0)
	{
		pv += 10;
		q = strchr(pv, '\'');
		if (q == NULL)
		{
			Mem_Free(vt);
			LOG_E("sql error [%s]", sql);
			return -1;
		}
		*q = '\0';
		vt->cdata = (char*)Mem_Malloc(q - pv + 1);
		if (vt->cdata == NULL)
		{
			Mem_Free(vt);
			LOG_E("malloc cdata error");
			return -1;
		}

		strcpy(vt->cdata, pv);
	}
	else
	{
		Mem_Free(vt);
		LOG_E("sql error[%s]", sql);
		return -1;
	}

	vt->next = NULL;
	for (r = vhead; r->next; r = r->next);

	r->next = vt;
	return 0;
}

static int find_UDT_sub(int col_index, int* pid, unsigned char* data, int len, UDT_NULL_INFO* uni)
{
	int				i;
	int				j;
	int				find = 0;

	//������ÿ����Ԫ��
	for (i = 0; i < g_col[col_index].parm_num; i++)
	{
		find = 0;
		for (j = 0; j < MAX_COL_NUM; j++)
		{
			if (g_col[j].used == 0)
			{
				break;
			}
			if (g_col[j].p_col_index != col_index)
			{
				continue;
			}
			if (g_col[j].sub_index == i+1)
			{
				//�ҵ���UDT����Ԫ��
				find = 1;
				break;
			}
		}
		if (find == 0)
		{
			LOG_E("can't find UDT's sub elem, name[%s] sub_index[%d]", g_col[col_index].name, i);
			return -1;
		}

		//����������UDT���н��м�¼
		if ((g_col[j].flag & COL_FLAG_VIRTUAL) == COL_FLAG_VIRTUAL)
		{
			uni[*pid].used = 1;
			uni[*pid].col_index = j;
			uni[*pid].is_null = ((data[*pid / 8] & (1 << (*pid % 8))) == 0) ? 0 : 1;
			(*pid)++;
			//����������ȵݹ�
			if (find_UDT_sub(j, pid, data, len, uni) < 0)
			{
				LOG_E("find SUB UDT info error [%s]", g_col[j].name);
				return -1;
			}
		}
	}
	return 0;
}

static int parse_UDT_NULL(DICT_COL* col, char* val, UDT_NULL_INFO* uni)
{
	int				id = 0;
	unsigned char	data[1024] = { 0 };
	int				datalen = 0;

	datalen = STR_HEX2BIN(val, data, 1024);

	id = 1;
	if (find_UDT_sub(col->self_index, &id, data, datalen, uni) < 0)
	{
		LOG_E("can not find sub UDT info, cid[%d] scid[%d] name[%s]", col->cid, col->scid, col->name);
		return -1;
	}

	return 0;
}

static void set_UDT_sub(DICT_COL* col, UDT_NULL_INFO* uni, int* id)
{
	int			j;

	if (col->parm_num == 0)
	{
		return;
	}

	uni[*id].used = 1;
	uni[*id].col_index = col->self_index;
	uni[*id].is_null = 1;
	(*id)++;

	for (j = 0; j < MAX_COL_NUM; j++)
	{
		if (g_col[j].used == 0)
		{
			break;
		}

		if (g_col[j].p_col_index == col->self_index)
		{
			set_UDT_sub(&g_col[j], uni, id);
		}
	}
}

static void set_UDT_NULL(DICT_COL* col, UDT_NULL_INFO* uni)
{
	int		id = 1;

	set_UDT_sub(col, uni, &id);
}

//1--��ʾNULL 0��ʾ��ΪNULL
static int get_parent_is_null(DICT_COL* col, UDT_NULL_INFO* uni)
{
	int			i;
	DICT_COL*	pcol;

	pcol = col;
	while (pcol->p_col_index != -1)
	{
		for (i = 1; i < MAX_UDT_NULL_NUM; i++)
		{
			if (uni[i].used == 0)
			{
				break;
			}
			if (uni[i].col_index != col->p_col_index)
			{
				continue;
			}
			//
			if (uni[i].is_null == 1)
			{
				return 1;
			}
		}

		pcol = &g_col[pcol->p_col_index];
	}

	return 0;
}

static int parse_old_image(char* sql, uint32_t objn, SQL_VECTOR* vhead, char* rowid)
{
	char*		pc;
	char*		pv;
	char*		pcn;
	char*		q;
	DICT_COL*	col = NULL;

	pc = sql;
	while (1)
	{
		pcn = strstr(pc, "and ");
		if (pcn != NULL)
		{
			*pcn = '\0';
		}
		
		//����ROWID
		if (strncmp(pc, "ROWID", 5) == 0)
		{
			pv = strchr(pc, '\'');
			pv += 1;
			q = strchr(pv, '\'');
			*q = '\0';
			strcpy(rowid, pv);

			if (pcn == NULL)
			{
				break;
			}
			pc = pcn + 4;
			continue;
		}
		//�ҵ�ֵ�Ŀ�ʼ �п����� = Ҳ�п����� IS NULL ���� IS NOT NULL
		pv = strchr(pc, '=');
		if (pv == NULL)
		{
			pv = strstr(pc, "IS ");
			if (pv == NULL)
			{
				LOG_E("parse sql error[%s]", sql);
				return -1;
			}
			pv += 3;
		}
		else
		{
			pv += 1;
		}

		if (parse_column(sql, pc, pv, objn, vhead, &col) < 0)
		{
			LOG_E("get_column_owner_tab error");
			return -1;
		}

		if (pcn == NULL)
		{
			break;
		}
		pc = pcn + 4;
	}
	return 0;
}

static int parse_where_image(SQL_VECTOR* old, SQL_VECTOR* vhead)
{
	SQL_VECTOR*	r;
	SQL_VECTOR*	r1;
	SQL_VECTOR*	vt;

	for (r = old->next; r; r = r->next)
	{
		if (((r->pcol->flag & COL_FLAG_PK) == COL_FLAG_PK) ||
			(r->pcol->flag & COL_FLAG_UK) == COL_FLAG_UK)
		{
			vt = (SQL_VECTOR*)Mem_Malloc(sizeof(SQL_VECTOR));
			if (vt == NULL)
			{
				LOG_E("malloc SQL_VECTOR error");
				return -1;
			}
			memset(vt, 0, sizeof(SQL_VECTOR));
			vt->ctype = r->ctype;
			vt->pcol = r->pcol;
			strcpy(vt->cname, r->cname);
			vt->flag = r->flag;
			vt->cdata = (char*)Mem_Malloc(strlen(r->cdata) + 1);		//�������r->cdata һ����ֵ
			if (vt->cdata == NULL)
			{
				Mem_Free(vt);
				LOG_E("malloc cdata error");
				return -1;
			}

			strcpy(vt->cdata, r->cdata);

			vt->next = NULL;
			for (r1 = vhead; r1->next; r1 = r1->next);

			r1->next = vt;

			break;		//ֻ��Ҫһ������
		}
	}
	return 0;
}

static int create_insert_sql(SQL_REC* rec, char** psql)
{
	int				i;
	char*			sql;
	char*			q;
	int				nlen = 0;
	SQL_VECTOR*		r;
	char			val[8192] = { 0 };
	char			typename[256] = { 0 };
	UDT_NULL_INFO	uni[MAX_UDT_NULL_NUM];

	g_top_null_index = -1;
	memset(uni, 0, sizeof(UDT_NULL_INFO)*MAX_UDT_NULL_NUM);

	//ƴ��sql���
	sql = Mem_Malloc(MAX_SQL_LEN);
	if (sql == NULL)
	{
		LOG_E("malloc sql error");
		return -1;
	}
	memset(sql, 0, MAX_SQL_LEN);
	q = sql;
	//�������Ϣ
	q += sprintf(sql, "insert into %s.%s(", rec->owner, rec->tabname);
	for (r = rec->new_vec.next; r; r = r->next)
	{
		if (r->pcol->p_col_index != -1)	
		{
			//���˵�UDT�е����У���p_col_index ��Ϊ-1���У�
			//����������һ����ĳ�е����� ����Ҳ����˵�
			continue;
		}
		q += sprintf(q, "%s,", r->cname);
	}

	*(q - 1) = ')';

	//���ֵ��Ϣ
	q += sprintf(q, " values(");
	for (r = rec->new_vec.next; r; r = r->next)
	{
		memset(val, 0, sizeof(val));
		get_column_value(r, val);

		if (r->pcol->parm_num != 0)
		{
			//�����UDT��
			//��Ҫ����UDT�е�NULL��־
			memset(uni, 0, sizeof(UDT_NULL_INFO)*MAX_UDT_NULL_NUM);
			if ((r->flag & VECTOR_FLAG_NULL) != VECTOR_FLAG_NULL)
			{
				//UDT�в�Ϊ��
				parse_UDT_NULL(r->pcol, val, uni);
			}
			else
			{
				//UDT��Ϊ��
				set_UDT_NULL(r->pcol, uni);
				
			}
			//�����ӡUDT�µ�NULL��־
			for (i = 1; i < MAX_UDT_NULL_NUM; i++)
			{
				if (uni[i].used == 0)
				{
					break;
				}

				LOG_D("uni info index[%d] isnull[%d]", uni[i].col_index, uni[i].is_null);
			}
			continue;
		}

		if (r->pcol->p_col_index != -1)
		{
			//��ʾ�����и���
			if (r->pcol->sub_index == 1)
			{
				//�Ǹ��е���ʼλ��
				memset(typename, 0, sizeof(typename));
				get_dict_column_typename_start(r->pcol, typename, uni);
				q += sprintf(q, "%s", typename);
			}
		}
		//�����ǻ�������
		if (((r->pcol->flag & COL_FLAG_ISARRAY) == COL_FLAG_ISARRAY)
			&& ((r->flag & VECTOR_FLAG_NULL) != VECTOR_FLAG_NULL))
		{
			//��������鲢�Ҳ�ΪNULL���������������
			q += sprintf(q, "%s(%s),", r->pcol->type_name, val);
		}
		else
		{
			//����ֵ
			if (get_parent_is_null(r->pcol, uni) != 1)
			{
				q += sprintf(q, "%s,", val);
			}
			//�����������NULL ����в�Ҫ
		}
		
		if (r->pcol->p_col_index != -1)
		{
			//��ʾ�����и���
			if (r->pcol->sub_index == g_col[r->pcol->p_col_index].parm_num)
			{
				//�Ǹ��е����λ��
				q -= 1;		//ȥ������
				memset(typename, 0, sizeof(typename));
				get_dict_column_typename_end(r->pcol, typename, uni);
				q += sprintf(q, "%s,", typename);
			}
		}
	}
	*(q - 1) = ')';


	nlen = strlen(sql);
	if (nlen > MAX_SQL_LEN)
	{
		LOG_E("over MAX_SQL_LEN sqllen=[%d]", nlen);
		Mem_Free(sql);
		return -1;
	}

	LOG_D("======>sql[%s]", sql);

	*psql = Mem_Malloc(nlen + 1);
	if (*psql == NULL)
	{
		LOG_E("malloc *psql error");
		Mem_Free(sql);
		return -1;
	}

	strcpy(*psql, sql);
	Mem_Free(sql);
	return 0;
}

static int create_update_sql(SQL_REC* rec, char** psql, char* rowid)
{
	char*			sql;
	char*			q;
	int				nlen = 0;
	SQL_VECTOR*		r;
	SQL_VECTOR*		r1;
	int				has_UDT_flag = 0;
	char			val[8192] = { 0 };
	int				find = 0;
	char			typename[1024] = { 0 };
	UDT_NULL_INFO	new_uni[MAX_UDT_NULL_NUM];
	UDT_NULL_INFO	old_uni[MAX_UDT_NULL_NUM];
	int				old_index = -1;
	int				new_index = -1;
	int				old_temp_index = -1;
	int				new_temp_index = -1;
	int				old_level = 0;
	int				new_level = 0;

	sql = Mem_Malloc(MAX_SQL_LEN);
	if (sql == NULL)
	{
		LOG_E("malloc sql error");
		return -1;
	}

	LOG_D("get update sql rowid=[%s]", rowid);
	//��ȡ��ǰ�е�����UDT�е�NULLֵ��Ϣ


	memset(sql, 0, MAX_SQL_LEN);
	q = sql;
	q += sprintf(q, "update %s.%s a set ", rec->owner, rec->tabname);

	for (r = rec->new_vec.next; r; r = r->next)
	{
		if (r->pcol->parm_num != 0)
		{
			has_UDT_flag = 1;
			//�����ײ���UDT���У������˵�����
			memset(new_uni, 0, sizeof(UDT_NULL_INFO)*MAX_UDT_NULL_NUM);
			if ((r->flag & VECTOR_FLAG_NULL) != VECTOR_FLAG_NULL)
			{
				//��ΪNULL
				memset(val, 0, sizeof(val));
				get_column_value(r, val);
				parse_UDT_NULL(r->pcol, val, new_uni);
				
			}
			else
			{
				//ΪNULL
				set_UDT_NULL(r->pcol, new_uni);
			}
			find = 0;
			memset(old_uni, 0, sizeof(UDT_NULL_INFO)*MAX_UDT_NULL_NUM);
			//���set ����UDT�� ����where��һ���ж�Ӧ�ĸ�UDT��
			//��where�еĶ�Ӧ��UDT����
			for (r1 = rec->old_vec.next; r1; r1 = r1->next)
			{
				if (r1->pcol->parm_num == 0)
				{
					continue;
				}
				if (r1->pcol->self_index == r->pcol->self_index)
				{
					find = 1;
					memset(val, 0, sizeof(val));
					get_column_value(r1, val);
					parse_UDT_NULL(r1->pcol, val, old_uni);
					break;
				}
			}
			if (find == 0)
			{
				LOG_E("not find UDT column in where image, new image UDT index[%d] name[%s]", r->pcol->self_index, r->pcol->name);
				return -1;
			}
			continue;
		}

		if (r->pcol->p_col_index == -1)
		{
			//����û�и��е���

			//����ģʽ
			q += sprintf(q, "a.%s = ", r->cname);
			get_column_value(r, val);
			
			if (((r->pcol->flag & COL_FLAG_ISARRAY) == COL_FLAG_ISARRAY)
				&& ((r->flag & VECTOR_FLAG_NULL) != VECTOR_FLAG_NULL))
			{
				//��������鲢�Ҳ�ΪNULL���������������
				q += sprintf(q, "%s(%s),", r->pcol->type_name, val);
			}
			else
			{
				q += sprintf(q, "%s,", val);
			}
		}
		else
		{
			//�и���
			if (r->pcol->sub_index == 1)
			{
				has_UDT_flag = 1;
				//�Ǹ��е���ʼλ��
				get_topmost_null_index(r->pcol, &old_temp_index, &old_level, old_uni);
				get_topmost_null_index(r->pcol, &new_temp_index, &new_level, new_uni);

				if (old_temp_index == -1 && new_temp_index == -1)
				{
					//��ʾ��������ģʽ
					LOG_D("normal mode...");
					new_index = -1;
					old_index = -1;
					q += sprintf(q, "a.%s = ", r->cname);
				}
				else if (old_temp_index == -1 && new_temp_index != -1)
				{
					//new ģʽ
					LOG_D("new mode..., old_index=[%d] new_index=[%d]", old_temp_index, new_temp_index);
					if ((new_index == -1)||((new_index != -1) && (new_index != new_temp_index)))
					{
						new_index = new_temp_index;
						memset(typename, 0, sizeof(typename));
						get_column_full_name(new_index, typename);
						q += sprintf(q, "a.%s = NULL,", typename);
					}
				}
				else if (old_temp_index != -1 && new_temp_index == -1)
				{
					//old ģʽ
					LOG_D("old mode..., old_index=[%d] new_index=[%d]", old_temp_index, new_temp_index);
					if ((old_index == -1) || ((old_index != -1) && (old_index != old_temp_index)))
					{
						old_index = old_temp_index;
						memset(typename, 0, sizeof(typename));
						get_column_full_name(old_index, typename);
						q += sprintf(q, "a.%s = ", typename);
					}
					memset(typename, 0, sizeof(typename));
					get_dict_column_typename_start_limit(r->pcol, typename, old_index);
					q += sprintf(q, "%s", typename);
				}
				else if (old_temp_index != -1 && new_temp_index != -1)
				{
					if (old_level <= new_level)			//������Ҫ�Ƚϲ㼶��ϵ
					{
						//new ģʽ
						LOG_D("new mode..., old_index=[%d] new_index=[%d]", old_temp_index, new_temp_index);
						if ((new_index == -1) || ((new_index != -1) && (new_index != new_temp_index)))
						{
							new_index = new_temp_index;
							memset(typename, 0, sizeof(typename));
							get_column_full_name(new_index, typename);
							q += sprintf(q, "a.%s = NULL,", typename);
						}
					}
					else
					{
						//��old ���޵� newģʽ
						LOG_D("old (new) mode..., old_index=[%d] new_index=[%d]", old_temp_index, new_temp_index);
						if ((old_index == -1) || ((old_index != -1) && (old_index != old_temp_index)))
						{
							old_index = old_temp_index;
							memset(typename, 0, sizeof(typename));
							get_column_full_name(old_index, typename);
							q += sprintf(q, "a.%s = ", typename);
							memset(typename, 0, sizeof(typename));
							get_dict_column_typename_start_end_limit(r->pcol, typename, new_temp_index, old_index);
							q += sprintf(q, "%s", typename);
						}
						
						if ((new_index == -1) || ((new_index != -1) && (new_index != new_temp_index)))
						{
							new_index = new_temp_index;
							q += sprintf(q, "NULL,");
						}
					}
				}
			}

			if (get_parent_is_null(r->pcol, new_uni) != 1)
			{
				get_column_value(r, val);
				if (((r->pcol->flag & COL_FLAG_ISARRAY) == COL_FLAG_ISARRAY)
					&& ((r->flag & VECTOR_FLAG_NULL) != VECTOR_FLAG_NULL))
				{
					//��������鲢�Ҳ�ΪNULL���������������
					q += sprintf(q, "%s(%s),", r->pcol->type_name, val);
				}
				else
				{
					q += sprintf(q, "%s,", val);
				}

				//�����������������ΪNULL �����ֵ����
			}
			
			if (r->pcol->sub_index == g_col[r->pcol->p_col_index].parm_num)
			{
				//�Ǹ��е����λ��
				q -= 1;		//ȥ������
				get_topmost_null_index(r->pcol, &old_temp_index, &old_level, old_uni);
				get_topmost_null_index(r->pcol, &new_temp_index, &new_level, new_uni);
				if (old_temp_index == -1 && new_temp_index == -1)
				{
					//����ģʽ
					q += 1;
				}
				else if (old_temp_index == -1 && new_temp_index != -1)
				{
					//new ģʽ
					q += 1;
				}
				else if (old_temp_index != -1 && new_temp_index == -1)
				{
					//old ģʽ
					memset(typename, 0, sizeof(typename));
					get_dict_column_typename_end_start_limit(r->pcol, typename, old_temp_index);
					q += sprintf(q, "%s,", typename);
				}
				else if (old_temp_index != -1 && new_temp_index != -1)
				{
					if (old_level <= new_level)			//������Ҫ�Ƚϲ㼶��ϵ
					{
						q += 1;
					}
					else
					{
						memset(typename, 0, sizeof(typename));
						get_dict_column_typename_end_start_end_limit(r->pcol, typename, new_temp_index, old_temp_index);
						q += sprintf(q, "%s,", typename);
					}
				}
			}
		}	
	}

	*(q - 1) = '\0';
	q -= 1;

	q += sprintf(q, " where ");

	if ((has_UDT_flag == 1) && (rec->where_vec.next == NULL))
	{
		LOG_E("the update sql has UDT, but not has PK or UK");
		return -1;
	}

	if(rec->where_vec.next != NULL)
	{
		for (r = rec->where_vec.next; r; r = r->next)
		{
			if (((r->flag & VECTOR_FLAG_NULL) == VECTOR_FLAG_NULL) ||
				((r->flag & VECTOR_FLAG_NOT_NULL) == VECTOR_FLAG_NOT_NULL))
			{
				q += sprintf(q, "a.%s IS ", r->cname);
			}
			else
			{
				q += sprintf(q, "a.%s = ", r->cname);
			}
			get_column_value(r, val);
			q += sprintf(q, "%s", val);
		}
	}
	else
	{
		for (r = rec->old_vec.next; r; r = r->next)
		{
			if (((r->flag & VECTOR_FLAG_NULL) == VECTOR_FLAG_NULL) ||
				((r->flag & VECTOR_FLAG_NOT_NULL) == VECTOR_FLAG_NOT_NULL))
			{
				q += sprintf(q, "a.%s IS ", r->cname);
			}
			else
			{
				if (r->pcol->parm_num != 0)
				{
					//ȥ���ײ���UDT����
					continue;
				}

				q += sprintf(q, "a.%s = ", r->cname);
			}


			get_column_value(r, val);
			q += sprintf(q, "%s and ", val);
		}
		*(q - 5) = '\0';
	}
	

	nlen = strlen(sql);
	if (nlen > MAX_SQL_LEN)
	{
		LOG_E("over MAX_SQL_LEN sqllen=[%d]", nlen);
		Mem_Free(sql);
		return -1;
	}

	LOG_D("======>sql[%s]", sql);

	*psql = Mem_Malloc(nlen + 1);
	if (*psql == NULL)
	{
		LOG_E("malloc *psql error");
		Mem_Free(sql);
		return -1;
	}

	strcpy(*psql, sql);
	Mem_Free(sql);
	return 0;
}

static int create_delete_sql(SQL_REC* rec, char** psql)
{
	char*			sql;
	char*			q;
	int				nlen = 0;
	SQL_VECTOR*		r;
	char			val[8192] = { 0 };
	int				has_UDT_flag = 0;

	sql = Mem_Malloc(MAX_SQL_LEN);
	if (sql == NULL)
	{
		LOG_E("malloc sql error");
		return -1;
	}
	memset(sql, 0, MAX_SQL_LEN);

	q = sql;
	q += sprintf(q, "delete %s.%s where ", rec->owner, rec->tabname);


	for (r = rec->old_vec.next; r; r = r->next)
	{
		if ((r->pcol->parm_num != 0) || (r->pcol->p_col_index != -1))
		{
			has_UDT_flag = 1;
			break;
		}
	}

	if ((has_UDT_flag == 1) && (rec->where_vec.next == NULL))
	{
		LOG_E("the delete sql has UDT, but not has PK or UK");
		return -1;
	}

	if (rec->where_vec.next != NULL)
	{
		for (r = rec->where_vec.next; r; r = r->next)
		{
			if (((r->flag & VECTOR_FLAG_NULL) == VECTOR_FLAG_NULL) ||
				((r->flag & VECTOR_FLAG_NOT_NULL) == VECTOR_FLAG_NOT_NULL))
			{
				q += sprintf(q, "%s IS ", r->cname);
			}
			else
			{
				q += sprintf(q, "%s = ", r->cname);
			}

			get_column_value(r, val);
			q += sprintf(q, "%s", val);
		}
	}
	else
	{
		for (r = rec->old_vec.next; r; r = r->next)
		{
			if (((r->flag & VECTOR_FLAG_NULL) == VECTOR_FLAG_NULL) ||
				((r->flag & VECTOR_FLAG_NOT_NULL) == VECTOR_FLAG_NOT_NULL))
			{
				q += sprintf(q, "%s IS ", r->cname);
			}
			else
			{
				q += sprintf(q, "%s = ", r->cname);
			}

			get_column_value(r, val);
			q += sprintf(q, "%s and ", val);
		}
		*(q - 4) = '\0';
	}

	nlen = strlen(sql);
	if (nlen > MAX_SQL_LEN)
	{
		LOG_E("over MAX_SQL_LEN sqllen=[%d]", nlen);
		return -1;
	}

	LOG_D("======>sql[%s]", sql);

	*psql = Mem_Malloc(nlen + 1);
	if (*psql == NULL)
	{
		LOG_E("malloc *psql error");
		return -1;
	}

	strcpy(*psql, sql);
	Mem_Free(sql);
	return 0;
}

static void free_rec_list(SQL_REC* rec)
{
	SQL_VECTOR*	r;
	SQL_VECTOR*	vt;

	if (rec == NULL)
	{
		return;
	}
	
	for (r = &rec->new_vec; r->next;)
	{
		vt = r->next;
		r->next = vt->next;
		if (vt->cdata)
		{
			Mem_Free(vt->cdata);
		}
		Mem_Free(vt);
	}

	for (r = &rec->old_vec; r->next;)
	{
		vt = r->next;
		r->next = vt->next;
		if (vt->cdata)
		{
			Mem_Free(vt->cdata);
		}
		Mem_Free(vt);
	}

	for (r = &rec->where_vec; r->next;)
	{
		vt = r->next;
		r->next = vt->next;
		if (vt->cdata)
		{
			Mem_Free(vt->cdata);
		}
		Mem_Free(vt);
	}
}

//���� insert ���
int parse_sql_insert(LC_INFO* info, char** psql)
{
	int			i;
	SQL_REC		rec;
	SQL_VECTOR*	r;
	SQL_VECTOR*	vt;
	char*		pc;
	char*		pv;
	char*		pcn;
	char*		pvn;
	int			nct = 0;
	
	
	DICT_COL*	col = NULL;
	DICT_COL*	m = NULL;
	

	memset(&rec, 0, sizeof(rec));

	rec.opc = info->opc;
	rec.scn = info->scn;
	rec.xid = info->xid;
	rec.thread = info->thread;
	if (odt_look_obj(info->objn, rec.owner, rec.tabname) < 0)
	{
		LOG_E("not found obj[%u] in dict", info->objn);
		return -1;
	}

	if ((pc = strchr(info->sql_redo, '(')) == NULL)
	{
		LOG_E("sql not find ( ... [%s]", info->sql_redo);
		return -1;
	}

	if ((pv = strstr(info->sql_redo, "values")) == NULL)
	{
		LOG_E("sql not find values ... [%s]", info->sql_redo);
		return -1;
	}

	pv += strlen("values");
	if ((pv = strchr(pv, '(')) == NULL)
	{
		LOG_E("sql not find values ( ... [%s]", info->sql_redo);
		return -1;
	}
	*pv = '\0';

	pc++;
	pv++;

	while (1)
	{
		pcn = strchr(pc, ',');
		pvn = strchr(pv, ',');

		if (parse_column(info->sql_redo, pc, pv, info->objn, &rec.new_vec, &col) < 0)
		{
			LOG_E("parse column obj[%u] error", info->objn);
			free_rec_list(&rec);
			return -1;
		}

		nct++;
		if(pcn == NULL)
		{
			break;
		}
		pv = pvn + 1;
		pc = pcn + 1;
	}

	//ʣ����м����ȥ ����Ϊ��
	for (i = 0; i < MAX_COL_NUM; i++)
	{
		if (g_col[i].used == 0)
		{
			break;
		}
		m = &g_col[i];
		if (m->p_tab_index != col->p_tab_index)
		{
			continue;
		}
		if (m->scid <= col->scid)
		{
			continue;
		}

		if ((m->flag & COL_FLAG_VIRTUAL) == COL_FLAG_VIRTUAL)		//���Ե�������
		{
			continue;
		}
		
		vt = (SQL_VECTOR*)Mem_Malloc(sizeof(SQL_VECTOR));
		if (vt == NULL)
		{
			LOG_E("malloc SQL_VECTOR error");
			free_rec_list(&rec);
			return -1;
		}
		memset(vt, 0, sizeof(SQL_VECTOR));
		vt->ctype = m->type;
		vt->pcol = m;
		strcpy(vt->cname, m->name);
		vt->flag = VECTOR_FLAG_NULL;			//����ΪNULL
		vt->next = NULL;
		for (r = &rec.new_vec; r->next; r = r->next);

		r->next = vt;
		nct++;
	}

	if (create_insert_sql(&rec, psql) < 0)
	{
		LOG_E("create insert sql error");
		free_rec_list(&rec);
		return -1;
	}

	free_rec_list(&rec);

	return 0;
}

int parse_sql_update(LC_INFO* info, char** psql)
{
	SQL_REC		rec;
	char*		pc;
	char*		pv;
	char*		pcn;
	char*		where_sql;
	DICT_COL*	col = NULL;
	char		rowid[64] = { 0 };

	memset(&rec, 0, sizeof(rec));

	rec.opc = info->opc;
	rec.scn = info->scn;
	rec.xid = info->xid;
	rec.thread = info->thread;
	if (odt_look_obj(info->objn, rec.owner, rec.tabname) < 0)
	{
		LOG_E("not found obj[%u] in dict", info->objn);
		return -1;
	}

	//����new image
	if ((pc = strstr(info->sql_redo, "set")) == NULL)
	{
		LOG_E("sql not find set ... [%s]", info->sql_redo);
		return -1;
	}

	pc += 3;

	if ((where_sql = strstr(info->sql_redo, "where")) == NULL)
	{
		LOG_E("sql not find where ... [%s]", info->sql_redo);
		return -1;
	}
	where_sql += 5;

	while (1)
	{
		pcn = strchr(pc, ',');
		pv = strchr(pc, '=');
		if (pv == NULL)
		{
			LOG_E("parse sql error[%s]", info->sql_redo);
			return -1;
		}
		pv++;

		if (parse_column(info->sql_redo, pc, pv, info->objn, &rec.new_vec, &col) < 0)
		{
			LOG_E("parse column obj[%u] error", info->objn);
			free_rec_list(&rec);
			return -1;
		}
		
		if (pcn == NULL)
		{
			break;
		}
		pc = pcn + 1;
	}

	//���� old image
	if (parse_old_image(where_sql, info->objn, &rec.old_vec, rowid) < 0)
	{
		LOG_E("parse column obj[%u] error", info->objn);
		free_rec_list(&rec);
		return -1;
	}

	//���� where image
	if (parse_where_image(&rec.old_vec, &rec.where_vec) < 0)
	{
		LOG_E("parse where image error");
		free_rec_list(&rec);
		return -1;
	}

	//����update���
	if (create_update_sql(&rec, psql, rowid) < 0)
	{
		LOG_E("create update sql error");
		free_rec_list(&rec);
		return -1;
	}

	free_rec_list(&rec);
	return 0;
}

int parse_sql_delete(LC_INFO* info, char** psql)
{
	SQL_REC		rec;
	char*		where_sql;
	char		rowid[64] = { 0 };

	memset(&rec, 0, sizeof(rec));

	rec.opc = info->opc;
	rec.scn = info->scn;
	rec.xid = info->xid;
	rec.thread = info->thread;
	if (odt_look_obj(info->objn, rec.owner, rec.tabname) < 0)
	{
		LOG_E("not found obj[%u] in dict", info->objn);
		return -1;
	}

	if ((where_sql = strstr(info->sql_redo, "where")) == NULL)
	{
		LOG_E("sql not find where ... [%s]", info->sql_redo);
		return -1;
	}
	where_sql += 5;

	//���� old image
	if (parse_old_image(where_sql, info->objn, &rec.old_vec, rowid) < 0)
	{
		free_rec_list(&rec);
		LOG_E("parse column obj[%u] error", info->objn);
		return -1;
	}

	//���� where image
	if (parse_where_image(&rec.old_vec, &rec.where_vec) < 0)
	{
		LOG_E("parse where image error");
		free_rec_list(&rec);
		return -1;
	}

	if(create_delete_sql(&rec, psql) < 0)
	{
		LOG_E("create_delete_sql error");
		free_rec_list(&rec);
		return -1;
	}

	free_rec_list(&rec);
	return 0;
}
