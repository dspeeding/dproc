#include "basetype.h"
#include "testcap.h"
#include "dict_opt.h"


extern DICT_LIST*	g_dict;
extern DICT_TAB*	g_tab;
extern DICT_COL*	g_col;

int		g_next_scid = 0;
int		g_new_vcol_flag = 0;
int		g_first_flag = 0;

static int get_col_typename(XOCI_CTX* ctx, char* toid, DICT_COL* col)
{
	int				ret;
	char			sql[1024] = { 0 };
	XOCI_STMT		stmt, *stmtp;
	int				rr1;
	int				rbuf_sz = 2 * 1024 * 1024;
	unsigned char*	rbuf;
	int				flag = 0;

	XOCI_DEFINE_SET ds;
	XOCI_DC			dc[4];
	char			owner[64];
	char			typename[64];

	stmtp = &stmt;
	memset(stmtp, 0, sizeof(XOCI_STMT));

	sprintf(sql, "select u.name,o.name,t.typecode,t.attributes from sys.type$ t,sys.obj$ o,sys.user$ u where o.oid$='%s' and o.OID$=t.TOID and o.owner#=u.user#", toid);

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_STR;	dc[0].vsz = 64;		//owner
	dc[1].flg = 0x03;	dc[1].dtyp = XOCI_STR;	dc[1].vsz = 64;		//name
	dc[2].flg = 0x03;	dc[2].dtyp = XOCI_INT;	dc[2].vsz = 4;		//typecode
	dc[3].flg = 0x03;	dc[3].dtyp = XOCI_INT;	dc[3].vsz = 4;		//attributes

	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error rbuf");
		xoci_close_statement(ctx, stmtp);
		return -1;
	}

	ds.nc = 4; ds.buf_sz = rbuf_sz; ds.buf = rbuf; ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		LOG_E("define for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		LOG_E("execute for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	ret = xoci_fetch(ctx, stmtp, 1, &rr1);
	if ((ret < 0) || (ret == XOCI_NO_DATA))
	{
		LOG_E("xoci_fetch for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	strcpy(owner, (char*)dc[0].valp);
	strcpy(typename, (char*)dc[1].valp);
	flag = *(int*)(dc[2].valp);
	col->parm_num = *(int*)(dc[3].valp);
	sprintf(col->type_name, "%s.%s", owner, typename);

	if (flag == 108)
	{
		col->flag |= COL_FLAG_ISUDT;
	}
	else if (flag == 122)
	{
		col->flag |= COL_FLAG_ISUDT;
		col->flag |= COL_FLAG_ISARRAY;
	}

	xoci_close_statement(ctx, stmtp);
	Mem_Free(rbuf);
	return 0;
}

static int get_col_oid(XOCI_CTX* ctx, DICT_COL* col)
{
	int			ret;
	char		sql[1024] = { 0 };
	XOCI_STMT	stmt, *stmtp;
	int			rr1;
	int			rbuf_sz = 2 * 1024 * 1024;
	unsigned char* rbuf;

	XOCI_DEFINE_SET ds;
	XOCI_DC			dc[1];

	stmtp = &stmt;
	memset(stmtp, 0, sizeof(XOCI_STMT));

	sprintf(sql, "select TOID from sys.coltype$ where obj#=%u and intcol#=%d", 
		g_tab[col->p_tab_index].tab_obj, col->scid);
	
	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_STR;	dc[0].vsz = 64;		//TOID

	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error rbuf");
		xoci_close_statement(ctx, stmtp);
		return -1;
	}

	ds.nc = 1; ds.buf_sz = rbuf_sz; ds.buf = rbuf; ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		LOG_E("define for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		LOG_E("execute for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	ret = xoci_fetch(ctx, stmtp, 1, &rr1);
	if ((ret < 0) || (ret == XOCI_NO_DATA))
	{
		LOG_E("xoci_fetch for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	strcpy(col->toid, (char*)dc[0].valp);

	xoci_close_statement(ctx, stmtp);
	Mem_Free(rbuf);
	return 0;
}

static void set_col_sub_name(int cur_index, int parent_index)
{
	char	name[128] = { 0 };

	sprintf(name, "%s.%s", g_col[parent_index].name, g_col[cur_index].name);
	strcpy(g_col[cur_index].name, name);

	if ((g_col[parent_index].flag & COL_FLAG_VIRTUAL) == COL_FLAG_VIRTUAL)
	{
		//递归的进行赋值
		set_col_sub_name(cur_index, g_col[parent_index].p_col_index);
	}
}

static void set_col_attr(char* attr_name, int attr_index, int col_index, int next_scid, DICT_COL* col)
{
	int		i;
	
	for (i = 0; i < MAX_COL_NUM; i++)
	{
		if (g_col[i].used == 0)
		{
			break;
		}

		if (g_col[i].p_tab_index != g_col[col_index].p_tab_index)
		{
			continue;
		}

		if (g_col[i].cid != g_col[col_index].cid)
		{
			continue;
		}

		if (g_col[i].scid == next_scid)
		{
//			LOG_D("find sub col index[%d] scid[%d]", i, g_col[i].scid);

			//找到该列
			g_col[i].flag = col->flag;
			strcpy(g_col[i].type_name, col->type_name);
			g_col[i].parm_num = 0;
			g_col[i].arr_type = col->arr_type;

			if (g_col[i].p_col_index == -1)
			{
				memset(g_col[i].name, 0, sizeof(g_col[i].name));
				strcpy(g_col[i].name, attr_name);
			}
			set_col_sub_name(i, col_index);
			g_col[i].p_col_index = col_index;
			g_col[i].sub_index = attr_index;
			break;
		}

		if ((g_first_flag == 0) && (g_col[i].scid == (next_scid - 1)))
		{
			//对于在最外侧的数组列的赋值
			g_col[i].flag = col->flag;
			strcpy(g_col[i].type_name, col->type_name);
			g_col[i].parm_num = 0;
			g_col[i].arr_type = col->arr_type;
			break;
		}
	}
}

static int get_attr_array(XOCI_CTX* ctx, char* toid, DICT_ATTR* attr, int* pattr_ct)
{
	int				ret;
	char			sql[1024] = { 0 };
	XOCI_STMT		stmt, *stmtp;
	int				rr1;
	int				rbuf_sz = 2 * 1024 * 1024;
	unsigned char*	rbuf;
	int				i;
	XOCI_DEFINE_SET ds;
	XOCI_DC			dc[3];
	int				attr_ct = 0;

	stmtp = &stmt;
	memset(stmtp, 0, sizeof(XOCI_STMT));

	sprintf(sql, "select NAME,ATTRIBUTE#,ATTR_TOID from sys.attribute$ where TOID='%s' order by ATTRIBUTE# asc", toid);

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_STR;	dc[0].vsz = 64;		//name
	dc[1].flg = 0x03;	dc[1].dtyp = XOCI_INT;	dc[1].vsz = 4;		//attribute#
	dc[2].flg = 0x03;	dc[2].dtyp = XOCI_STR;	dc[2].vsz = 64;		//attr_toid

	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error rbuf");
		xoci_close_statement(ctx, stmtp);
		return -1;
	}

	ds.nc = 3; ds.buf_sz = rbuf_sz; ds.buf = rbuf; ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		LOG_E("define for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		LOG_E("execute for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}
	while (1)
	{
		rr1 = 0;
		if ((ret = xoci_fetch(ctx, stmtp, ds.nrows, &rr1)) < 0)
		{
			LOG_E("xoci_fetch for table select error[%d-%s]", excp_get_code(), excp_get_message());
			xoci_close_statement(ctx, stmtp);
			Mem_Free(rbuf);
			return -1;
		}
		//LOG_D("fetch [%d]", rr1);

		for (i = 0; i < rr1; i++)
		{
			strcpy(attr[attr_ct].name, (char*)((char*)dc[0].valp + 64 * i));
			attr[attr_ct].attr_index = *((int*)dc[1].valp + i);
			strcpy(attr[attr_ct].attr_oid, (char*)((char*)dc[2].valp + 64 * i));
			attr_ct++;
		}

		if (ret == XOCI_NO_DATA)
		{
			break;
		}
	}

	*pattr_ct = attr_ct;

	xoci_close_statement(ctx, stmtp);
	Mem_Free(rbuf);

	return 0;
}

static int add_virtual_col(char* attr_name, int attr_index, char* attr_oid, int parent_index, int* pv_col_index)
{
	int			i;
	DICT_COL*	pc;

	for (i = 0; i < MAX_COL_NUM; i++)
	{
		if (g_col[i].used == 0)
		{
			break;
		}
	}

	if (i >= MAX_COL_NUM)
	{
		LOG_E("reach the max col num");
		return -1;
	}

	pc = &g_col[i];
	*pv_col_index = i;
	pc->used = 1;
	pc->flag = COL_FLAG_VIRTUAL;
	pc->cid = g_col[parent_index].cid;
	pc->scid = 0;
	pc->type = 121;
	pc->len = 0;
	strcpy(pc->name, attr_name);
	strcpy(pc->toid, attr_oid);

	pc->p_tab_index = g_col[parent_index].p_tab_index;
	pc->p_sch_index = g_col[parent_index].p_sch_index;
	pc->p_col_index = parent_index;
	pc->sub_index = attr_index;			//子列序号
	pc->self_index = i;

	return 0;
}

static int get_varray_type(XOCI_CTX* ctx, char* arr_toid, uint32_t* ptype)
{
	int				ret;
	char			sql[1024] = { 0 };
	XOCI_STMT		stmt, *stmtp;
	int				rr1;
	int				rbuf_sz = 2 * 1024 * 1024;
	unsigned char*	rbuf;
	XOCI_DEFINE_SET ds;
	XOCI_DC			dc[2];
	char			name[64];

	stmtp = &stmt;
	memset(stmtp, 0, sizeof(XOCI_STMT));

	sprintf(sql, "select t.typecode,o.name from sys.collection$ c,sys.obj$ o,sys.type$ t where c.TOID='%s' and c.ELEM_TOID=t.TOID and o.oid$ = t.TOID",
		arr_toid);

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_INT;	dc[0].vsz = 4;		//typecode
	dc[1].flg = 0x03;	dc[1].dtyp = XOCI_STR;	dc[1].vsz = 64;		//name

	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error rbuf");
		xoci_close_statement(ctx, stmtp);
		return -1;
	}

	ds.nc = 2; ds.buf_sz = rbuf_sz; ds.buf = rbuf; ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		LOG_E("define for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		LOG_E("execute for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	ret = xoci_fetch(ctx, stmtp, 1, &rr1);
	if ((ret < 0) || (ret == XOCI_NO_DATA))
	{
		LOG_E("xoci_fetch for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	*ptype = *(uint32_t*)dc[0].valp;
	strcpy(name, (char*)dc[1].valp);
//	LOG_D("name=[%s] type=[%u]", name, *ptype);

	xoci_close_statement(ctx, stmtp);
	Mem_Free(rbuf);
	return 0;
}

//该函数分为三个分支
//1、获取UDT列的基本信息并遍历其递归每个属性
//2、对于基本列，则配置其父列等信息 递归结束条件
//3、如果不是基本类型，则创建新的虚拟列 并递归调用2
static int get_attribute(XOCI_CTX* ctx, char* toid, int cur_index, char* attr_name, int attr_index)
{
	int				i;
	DICT_ATTR		attr[MAX_ATTR_NUM];
	int				attr_ct = 0;
	int				v_col_index = -1;
	int				temp_index = 0;
	DICT_COL		temp_col;

	memset(attr, 0, sizeof(DICT_ATTR)*MAX_ATTR_NUM);
	memset(&temp_col, 0, sizeof(DICT_COL));

//	LOG_D("%s start...", __FUNCTION__);
//	LOG_D("param toid[%s] cur_index[%d]", toid, cur_index);

	if (get_col_typename(ctx, toid, &temp_col) < 0)
	{
		LOG_E("get_col_typename error");
		return -1;
	}

//	LOG_D("get toid[%s] typename[%s] attr[%d] flag[%#x]", toid, temp_col.type_name, temp_col.parm_num, temp_col.flag);

	if ((temp_col.flag & COL_FLAG_ISARRAY) == COL_FLAG_ISARRAY)
	{
		//如果是array类型
//		LOG_D("col index[%d] is varray", cur_index);
		if (get_varray_type(ctx, toid, &temp_col.arr_type) < 0)
		{
			LOG_E("get_varray_type error index[%d] toid[%s]", cur_index, toid);
			return -1;
		}
	}

	if (temp_col.parm_num == 0)		//表示基本列
	{
		set_col_attr(attr_name, attr_index, cur_index, g_next_scid, &temp_col);
		g_next_scid++;
		return 0;
	}

	if (get_attr_array(ctx, toid, attr, &attr_ct) < 0)
	{
		LOG_E("get_attr_array error");
		return -1;
	}
//	LOG_D("col index[%d] scid[%d] name[%s] attr[%d]", cur_index, g_col[cur_index].scid,
//		g_col[cur_index].name, attr_ct);

	g_first_flag = 1;

	if (g_new_vcol_flag == 0)
	{
		//获取UDT类型的基本信息
		g_col[cur_index].flag |= temp_col.flag;
		strcpy(g_col[cur_index].type_name, temp_col.type_name);
		g_col[cur_index].parm_num = temp_col.parm_num;

		g_new_vcol_flag = 1;

		//对于获取到的每一个数据递归进行操作
		for (i = 0; i < attr_ct; i++)
		{
			if (get_attribute(ctx, attr[i].attr_oid, cur_index, attr[i].name, attr[i].attr_index) < 0)
			{
				LOG_E("get_attribute err, [%s] [%s]", g_tab[g_col[temp_index].p_tab_index].tabname, g_col[temp_index].name);
				return -1;
			}
		}
	}
	else
	{
		//新建一个虚拟列并递归
		if (add_virtual_col(attr_name, attr_index, toid, cur_index, &v_col_index) < 0)
		{
			LOG_E("add_virtual_col error");
			return -1;
		}
//		LOG_D("add virtual col index[%d] ", v_col_index);

		g_new_vcol_flag = 0;

		if (get_attribute(ctx, toid, v_col_index, attr_name, attr_index) < 0)
		{
			LOG_E("get_attribute err, [%s] [%s]", g_tab[g_col[temp_index].p_tab_index].tabname, g_col[temp_index].name);
			return -1;
		}
	}

	return 0;
}

int get_UDT_dict_info(XOCI_CTX* ctx)
{
	int		i;

	LOG_D("get UDT dict info start...");

	for (i = 0; i < MAX_COL_NUM; i++)
	{
		if (g_col[i].used == 0)
		{
			break;
		}

		if (!((g_col[i].type == 121) || (g_col[i].type == 122) || (g_col[i].type == 123)))
		{
			continue;
		}

		if (g_col[i].p_col_index != -1)		//表示已经解析过 是某个列的子列
		{
			continue;
		}

		//这里都是UDT类型了
		if (get_col_oid(ctx, &g_col[i]) < 0)
		{
			LOG_E("get_UDT_info_sub error,[%s] [%s]", g_tab[g_col[i].p_tab_index].tabname, g_col[i].name);
			return -1;
		}
//		LOG_D("begin analysis col index[%d] scid[%d] name[%s] oid[%s]", i, g_col[i].scid, g_col[i].name, g_col[i].toid);

		g_next_scid = g_col[i].scid + 1;
		g_new_vcol_flag = 0;
		g_first_flag = 0;
		if (get_attribute(ctx, g_col[i].toid, i, (char*)"", 0) < 0)
		{
			LOG_E("get_attribute err, [%s] [%s]", g_tab[g_col[i].p_tab_index].tabname, g_col[i].name);
			return -1;
		}
	}

	LOG_D("get UDT dict info end...");

	return 0;

}

static int save_dict_list(XOCI_DC* dc, int schema_index, int i)
{
	int			j;
	int			k;
	DICT_TAB*	pt;
	DICT_COL*	pc;
	uint32_t	objn;

	objn = *((uint32_t *)dc[0].valp + i);

	for (j = 0; j < MAX_TAB_NUM; j++)
	{
		if (g_tab[j].used == 0)
		{
			break;
		}
		if (g_tab[j].tab_obj == objn)
		{
			break;
		}
	}
	if (j >= MAX_TAB_NUM)
	{
		LOG_E("reach the max tab num");
		return -1;
	}

	pt = &g_tab[j];

	//如果是没有匹配的，则赋值新的
	if (pt->used == 0)
	{
		pt->tab_obj = objn;
		strcpy(pt->tabname, ((char*)dc[1].valp + 32 * i));
		pt->schema_index = schema_index;
		pt->flag = 0;
		pt->ncol = 0;
		pt->used = 1;
	}

	for (k = 0; k < MAX_COL_NUM; k++)
	{
		if (g_col[k].used == 0)
		{
			break;
		}
	}
	if (k >= MAX_COL_NUM)
	{
		LOG_E("reach the max col num");
		return -1;
	}

	pc = &g_col[k];
	pc->used = 1;

	pc->flag = 0;
	pc->cid = *((uint32_t*)dc[2].valp + i);
	pc->scid = *((uint32_t*)dc[3].valp + i);
	pc->type = *((uint32_t*)dc[4].valp + i);
	pc->len = *((uint32_t*)dc[5].valp + i);
	pc->csfrm = *((uint32_t*)dc[6].valp + i);
	strcpy(pc->name, ((char*)dc[7].valp + 32 * i));
	pc->deflen = *((uint32_t*)dc[8].valp + i);
	pc->null = *((uint32_t*)dc[9].valp + i);

	pc->p_sch_index = schema_index;
	pc->p_col_index = -1;
	pc->p_tab_index = j;
	pc->parm_num = 0;
	pc->sub_index = 0;
	pc->self_index = k;

	return 0;
}

int get_dict(XOCI_CTX* ctx, const char* schema)
{
	int			ret;
	char		sql[1024] = { 0 };
	XOCI_STMT	stmt, *stmtp;
	int			rr1;
	int			rbuf_sz = 2 * 1024 * 1024;
	unsigned char* rbuf;

	XOCI_DEFINE_SET ds;
	XOCI_DC			dc[10];
	int			i;
	int			schema_index = -1;

	//找位置存下来schema
	for (i = 0; i < MAX_SCHEMA_NUM; i++)
	{
		if (g_dict[i].schema[0] != '\0')
		{
			continue;
		}
		schema_index = i;
		strcpy(g_dict[i].schema, schema);
		break;
	}

	if (schema_index == -1)
	{
		LOG_E("already full schema list");
		return -1;
	}

	stmtp = &stmt;
	memset(stmtp, 0, sizeof(XOCI_STMT));

	sprintf(sql, "select t.obj#,o.name,c.col#,c.segcol#,c.type#,c.length,"
			"c.charsetform,c.name,nvl(c.deflength,0),null$ "
			"from sys.tab$ t,sys.user$ u,sys.obj$ o,sys.col$ c "
			"where t.obj#=o.obj# and o.owner#=u.user# and bitand(t.property,1)=0 "
			"and bitand(o.flags,128)=0 and bitand(o.flags,2)<>2 and c.obj#=o.obj# "
			"and c.col#>0 and u.name='%s' order by t.obj#, decode(segcol#,0,65535,segcol#), intcol#",
			schema);

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_INT;	dc[0].vsz = 4;		//obj#
	dc[1].flg = 0x03;	dc[1].dtyp = XOCI_STR;	dc[1].vsz = 32;		//table name
	dc[2].flg = 0x03;	dc[2].dtyp = XOCI_INT;	dc[2].vsz = 4;		//col#
	dc[3].flg = 0x03;	dc[3].dtyp = XOCI_INT;	dc[3].vsz = 4;		//segcol#
	dc[4].flg = 0x03;	dc[4].dtyp = XOCI_INT;	dc[4].vsz = 4;		//type#
	dc[5].flg = 0x03;	dc[5].dtyp = XOCI_INT;	dc[5].vsz = 4;		//length
	dc[6].flg = 0x03;	dc[6].dtyp = XOCI_INT;	dc[6].vsz = 4;		//charsetform
	dc[7].flg = 0x03;	dc[7].dtyp = XOCI_STR;	dc[7].vsz = 32;		//column name
	dc[8].flg = 0x03;	dc[8].dtyp = XOCI_INT;	dc[8].vsz = 4;		//column default length
	dc[9].flg = 0x03;	dc[9].dtyp = XOCI_INT;	dc[9].vsz = 4;		//column null$

	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error rbuf");
		xoci_close_statement(ctx, stmtp);
		return -1;
	}

	ds.nc = 10; ds.buf_sz = rbuf_sz; ds.buf = rbuf; ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		LOG_E("define for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		LOG_E("execute for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	while (1)
	{
		rr1 = 0;
		if ((ret = xoci_fetch(ctx, stmtp, ds.nrows, &rr1)) < 0)
		{
			LOG_E("xoci_fetch for table select error[%d-%s]", excp_get_code(), excp_get_message());
			xoci_close_statement(ctx, stmtp);
			Mem_Free(rbuf);
			return -1;
		}
		for (i = 0; i < rr1; i++)
		{
			save_dict_list(dc, schema_index, i);
		}
		if (ret == XOCI_NO_DATA)
		{
			break;
		}
	}

	xoci_close_statement(ctx, stmtp);
	Mem_Free(rbuf);
	return 0;
}

int get_dict_pk(XOCI_CTX* ctx, const char* schema)
{
	int				ret;
	char			sql[1024] = { 0 };
	XOCI_STMT		stmt, *stmtp;
	int				rr1;
	int				rbuf_sz = 2 * 1024 * 1024;
	unsigned char*	rbuf;

	XOCI_DEFINE_SET ds;
	XOCI_DC			dc[3];
	int				i;
	int				schema_index = -1;
	int				j;
	uint32_t		objn;
	uint32_t		cid;
	uint32_t		type;

	//找位置存下来schema
	for (i = 0; i < MAX_SCHEMA_NUM; i++)
	{
		if (g_dict[i].schema[0] == '\0')
		{
			break;
		}
		if (strcmp(g_dict[i].schema, schema) == 0)
		{
			schema_index = i;
			break;
		}
	}

	if (schema_index == -1)
	{
		LOG_E("not find schema in dict list, schema[%s]", schema);
		return -1;
	}

	stmtp = &stmt;
	memset(stmtp, 0, sizeof(XOCI_STMT));

	sprintf(sql, "select o.obj#, cc.col#,cd.type# from sys.obj$ o,sys.user$ u,sys.cdef$ cd,sys.ccol$ cc "
		"where cd.obj#=cc.obj# and cd.con#=cc.con# and (cd.type#=2 or cd.type#=3) and cd.enabled is not null "
		"and cc.obj#=o.obj# and o.owner#=u.user# and u.name='%s'", schema);

	if (xoci_prepare_statement(ctx, stmtp, sql) < 0)
	{
		LOG_E("xoci_prepare_statement err[%d-%s]", excp_get_code(), excp_get_message());
		return -1;
	}

	dc[0].flg = 0x03;	dc[0].dtyp = XOCI_INT;	dc[0].vsz = 4;		//obj#
	dc[1].flg = 0x03;	dc[1].dtyp = XOCI_INT;	dc[1].vsz = 4;		//col#
	dc[2].flg = 0x03;	dc[2].dtyp = XOCI_INT;	dc[2].vsz = 4;		//type#

	rbuf = Mem_Malloc(rbuf_sz);
	if (rbuf == NULL)
	{
		LOG_E("malloc error rbuf");
		xoci_close_statement(ctx, stmtp);
		return -1;
	}

	ds.nc = 3; ds.buf_sz = rbuf_sz; ds.buf = rbuf; ds.dc = dc;

	if (xoci_define(ctx, stmtp, &ds) < 0)
	{
		LOG_E("define for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	if (xoci_execute(ctx, stmtp, 0) < 0)
	{
		LOG_E("execute for table select error[%d-%s]", excp_get_code(), excp_get_message());
		xoci_close_statement(ctx, stmtp);
		Mem_Free(rbuf);
		return -1;
	}

	while (1)
	{
		rr1 = 0;
		if ((ret = xoci_fetch(ctx, stmtp, ds.nrows, &rr1)) < 0)
		{
			LOG_E("xoci_fetch for table select error[%d-%s]", excp_get_code(), excp_get_message());
			xoci_close_statement(ctx, stmtp);
			Mem_Free(rbuf);
			return -1;
		}
		for (i = 0; i < rr1; i++)
		{
			objn = *((int32_t*)dc[0].valp + i);
			cid = *((int32_t*)dc[1].valp + i);
			type = *((int32_t*)dc[2].valp + i);

			for (j = 0; j < MAX_COL_NUM; j++)
			{
				if (g_col[j].used == 0)
				{
					break;
				}
				if (g_col[j].p_sch_index != schema_index)
				{
					continue;
				}
				if (g_tab[g_col[j].p_tab_index].tab_obj != objn)
				{
					continue;
				}
				if (g_col[j].cid != cid)
				{
					continue;
				}
				if (type == 2)
				{
					g_col[j].flag |= COL_FLAG_PK;
				}
				else if (type == 3)
				{
					g_col[j].flag |= COL_FLAG_UK;
				}
				break;
			}
		}
		if (ret == XOCI_NO_DATA)
		{
			break;
		}
	}

	xoci_close_statement(ctx, stmtp);
	Mem_Free(rbuf);
	return 0;
}

void show_dict(void)
{
	int			k;
	DICT_LIST*	sch;
	DICT_TAB*	pt;
	DICT_COL*	pc;

	LOG_D("show dict...");


	for (k = 0; k < MAX_COL_NUM; k++)
	{
		if (g_col[k].used == 0)
		{
			break;
		}
		pc = &g_col[k];

		sch = &g_dict[pc->p_sch_index];
		pt = &g_tab[pc->p_tab_index];
		LOG_D("[%03d] sch[%8s] obj[%6u] tab[%8s] flg[%04X] cid[%3u] scid[%3u] type[%3u] len[%4u] csfrm[%2u] "
			"name[%16s] dlen[%3u] null[%2u] pcol[%03d] attr[%02d] sub[%02d] arrtype[%u] typename[%s]",
			k, sch->schema, pt->tab_obj, pt->tabname, 
			pc->flag, pc->cid, pc->scid, pc->type, pc->len, pc->csfrm,
			pc->name, pc->deflen, pc->null,
			pc->p_col_index, pc->parm_num, pc->sub_index, pc->arr_type, pc->type_name);
	}

}

int odt_lookup_schema(char* owner)
{
	int			i;
	DICT_LIST*	sch;

	for (i = 0; i < MAX_SCHEMA_NUM; i++)
	{
		if (g_dict[i].schema[0] == '\0')
		{
			break;
		}

		sch = &g_dict[i];
		if (strcmp(sch->schema, owner) == 0)
		{
			return i;
		}
	}
	return -1;
}

int odt_check_obj(uint32_t objn)
{
	int				j;

	for (j = 0; j < MAX_TAB_NUM; j++)
	{
		if (g_tab[j].used == 0)
		{
			break;
		}

		if (g_tab[j].tab_obj == objn)
		{
			return 0;
		}
	}

	return -1;
}

int odt_look_obj(uint32_t objn, char* schema, char* tabname)
{
	int			j;
	DICT_TAB*	pt;

	for (j = 0; j < MAX_TAB_NUM; j++)
	{
		if (g_tab[j].used == 0)
		{
			break;
		}
		pt = &g_tab[j];

		if (pt->tab_obj == objn)
		{
			strcpy(schema, g_dict[pt->schema_index].schema);
			strcpy(tabname, pt->tabname);
			return 0;
		}
	}

	return -1;
}

int odt_lookup_colobj(uint32_t objn, int scid, DICT_COL** pcol)
{
	int			j;
	int			k;
	int			find = 0;
	DICT_TAB*	pt;
	DICT_COL*	pc;

	for (j = 0; j < MAX_TAB_NUM; j++)
	{
		if (g_tab[j].used == 0)
		{
			break;
		}
		pt = &g_tab[j];

		if (pt->tab_obj == objn)
		{
			find = 1;
			break;
		}
	}

	if (find == 0)
	{
		LOG_E("not find obj[%u] in dict", objn);
		return -1;
	}

	for (k = 0; k < MAX_COL_NUM; k++)
	{
		if (g_col[k].used == 0)
		{
			break;
		}
		pc = &g_col[k];
		if (pc->p_tab_index != j)
		{
			continue;
		}
		if (pc->scid == scid)
		{
			find = 1;
			break;
		}
	}

	if (find == 0)
	{
		LOG_E("not find scid[%d] in tab[%s] dict", scid, pt->tabname);
		return -1;
	}

	*pcol = pc;
	return 0;
}

