#pragma once


#define MAX_ATTR_NUM			100					//һ��UDT�±�������Ը���

#define COL_FLAG_ISUDT			0x00000001			//��UDT����
#define COL_FLAG_ISARRAY		0x00000002			//��ARRAY����

#define COL_FLAG_VIRTUAL		0x00000004			//����������

#define COL_FLAG_PK				0x00000010			//PK
#define COL_FLAG_UK				0x00000020			//UK


typedef struct DICT_COL_ST
{
	uint32_t	flag;				//
	int			used;				//�Ƿ�ʹ�� 0--δ��ʹ�� 1--��ʹ��
	uint32_t	cid;				//cid
	uint32_t	scid;				//sicd
	uint32_t	type;				//type ����
	uint32_t	len;				//����
	uint32_t	csfrm;				//csfrm
	char		name[32];			//������
	uint32_t	deflen;				//Ĭ�ϳ���
	uint32_t	null;				//�Ƿ�Ϊ��
	int			p_sch_index;		//�����û���index
	int			p_tab_index;		//�������index
	int			self_index;			//�Լ����ڵ�index
	//UDT֧��
	int			p_col_index;		//�������е�index
	int			parm_num;			//���Ը���,���û��������Ϊ0
	int			sub_index;			//������ţ��������е�Ĭ��Ϊ0
	char		type_name[128];		//��������
	char		toid[64];			//�е�toid
	uint32_t	arr_type;			//�����е�����
}DICT_COL;

typedef struct DICT_TAB_ST
{
	uint32_t	flag;
	int			used;				//�Ƿ�ʹ�� 0--δ��ʹ�� 1--��ʹ��
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



