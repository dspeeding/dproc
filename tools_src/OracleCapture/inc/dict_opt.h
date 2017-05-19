#pragma once


#define MAX_ATTR_NUM			100					//一个UDT下边最大属性个数

#define COL_FLAG_ISUDT			0x00000001			//是UDT类型
#define COL_FLAG_ISARRAY		0x00000002			//是ARRAY类型

#define COL_FLAG_VIRTUAL		0x00000004			//是虚拟类型

#define COL_FLAG_PK				0x00000010			//PK
#define COL_FLAG_UK				0x00000020			//UK


typedef struct DICT_COL_ST
{
	uint32_t	flag;				//
	int			used;				//是否被使用 0--未被使用 1--被使用
	uint32_t	cid;				//cid
	uint32_t	scid;				//sicd
	uint32_t	type;				//type 类型
	uint32_t	len;				//长度
	uint32_t	csfrm;				//csfrm
	char		name[32];			//列名称
	uint32_t	deflen;				//默认长度
	uint32_t	null;				//是否为空
	int			p_sch_index;		//所属用户的index
	int			p_tab_index;		//所属表的index
	int			self_index;			//自己所在的index
	//UDT支持
	int			p_col_index;		//所属父列的index
	int			parm_num;			//属性个数,如果没有子列则为0
	int			sub_index;			//子列序号，不是子列的默认为0
	char		type_name[128];		//类型名称
	char		toid[64];			//列的toid
	uint32_t	arr_type;			//数组中的类型
}DICT_COL;

typedef struct DICT_TAB_ST
{
	uint32_t	flag;
	int			used;				//是否被使用 0--未被使用 1--被使用
	char		tabname[64];
	uint32_t	schema_index;
	uint32_t	tab_obj;
	uint32_t	ncol;
}DICT_TAB;

typedef struct DICT_LIST_ST
{
	char		schema[64];
	uint32_t	ntab;
}DICT_LIST;



typedef struct DICT_ATTR_ST
{
	char		name[64];
	int			attr_index;
	char		attr_oid[64];
}DICT_ATTR;

int get_dict(XOCI_CTX* ctx, const char* schema);

int get_dict_pk(XOCI_CTX* ctx, const char* schema);

int get_UDT_dict_info(XOCI_CTX* ctx);

void show_dict(void);

int odt_lookup_schema(char* owner);

int odt_check_obj(uint32_t objn);

int odt_look_obj(uint32_t objn, char* schema, char* tabname);

int odt_lookup_colobj(uint32_t objn, int scid, DICT_COL** pcol);



