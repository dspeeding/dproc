#include "basetype.h"
#include <math.h>
#include "type_transfer.h"
#include "dict_opt.h"

//格式为 sign bit/exponent, digit1,digit2...
//exponent 由3部分组成 sign bit, offset , exponent
//sign bit:根据第一个字节最高位是1还是0决定是正数还是负数
//offset恒等于65 
//exponent 的值区间为-65~62 为基于100为底的指数形式
//如果第一个字节的值大于等于128 则为正数，exponent=第一个字节的值-128-65 = 第一个字节的值-193
//这样可根据exponent依次减1，与digit减1的值相乘，最后相加即可得出实际值
//如果第一个字节的值小于128，则为负数 exponent = (255-第一个字节的值)-128-65 = 62-第一个字节的值。
//这样可根据exponent依次减1，与digit减1的值相乘（最后一个digit是102为标记不参与计算）然后相加，最后加上负号即可得出实际值
int tt_number(char* data, char* pval)
{
	int			i;
	int			n;
	int			tmp;
	int			flag = 0;
	uint8_t		buf[256];
	uint8_t		str[256];
	uint8_t*	s;
	uint8_t		C1, C2, C3;
	int			len = 0;

	len = STR_HEX2BIN(data, buf, sizeof(buf));

	if (len == 0)
	{
		*pval = '\0';
		return 0;
	}

	memset(str, 0, sizeof(str));
	s = str;
	C1 = 0xC1;
	C2 = 0x3E;
	C3 = 0x80;

	if (buf[0] == C3)
	{
		*pval++ = '0';
		*pval = '\0';
		return 0;
	}

	if (buf[0] >= 0xC0)
	{
		n = (int)(buf[0] - C1);
		if (n >= 0)
		{
			tmp = (buf[1] - 1);
			if (tmp >= 10)
			{
				memset(s, 0x30, n * 2 + 2);
				*s++ = tmp / 10 + 0x30;
				*s++ = tmp % 10 + 0x30;
			}
			else
			{
				memset(s, 0x30, n * 2 + 1);
				*s++ = tmp + 0x30;
			}

			for (i = 2; i < len; i++)
			{
				if ((n - i + 1) < 0)
				{
					if (flag == 0)
					{
						*s++ = '.';
						flag = 1;
					}
				}
				tmp = (buf[i] - 1);
				*s++ = tmp / 10 + 0x30;
				*s++ = tmp % 10 + 0x30;
			}
		}
		else
		{
			*s++ = '0';
			*s++ = '.';
			flag = 1;
			for (i = 1; i < len; i++)
			{
				tmp = (buf[i] - 1);
				*s++ = tmp / 10 + 0x30;
				*s++ = tmp % 10 + 0x30;
			}
		}
	}
	else if (buf[0] <= 0x3F)
	{
		n = (int)(C2 - buf[0]);
		if (n >= 0)
		{
			tmp = (101 - buf[1]);
			if (tmp >= 10)
			{
				*s++ = '-';
				*s++ = tmp / 10 + 0x30;
				*s++ = tmp % 10 + 0x30;
			}
			else
			{
				*s++ = '-';
				*s++ = tmp + 0x30;
			}

			for (i = 2; i < len - 1; i++)
			{
				if ((n - i + 1) < 0)
				{
					if (flag == 0)
					{
						*s++ = '.';
						flag = 1;
					}
				}

				tmp = (101 - buf[i]);
				*s++ = tmp / 10 + 0x30;
				*s++ = tmp % 10 + 0x30;
			}
		}
		else
		{
			*s++ = '-';
			*s++ = '0';
			*s++ = '.';
			flag = 1;
			for (i = 1; i < len - 1; i++)
			{
				tmp = (101 - buf[i]);
				*s++ = tmp / 10 + 0x30;
				*s++ = tmp % 10 + 0x30;
			}
		}
	}
	if (flag == 1)
	{
		s--;
		if ((*s == '.') || (*s == '0'))
		{
			*s = '\0';
		}
	}
	strcpy(pval, (char*)str);
	return 0;
}

void tt_varchar2(char* data, char* val, int max_len)
{
	STR_HEX2BIN(data, (unsigned char*)val, max_len);
}

void tt_char(char* data, char* val, int max_len)
{
	STR_HEX2BIN(data, (unsigned char*)val, max_len);
	STR_Trim(val, TRIM_RIGHT);
}

//解析varray类型
//0001000000000001000000156ec1 0020 090000000000 0010 000000000001 8801 10 0101 0003 02c10202c10302c104
int tt_varray(char* data, DICT_COL* col, char* val, int max_len)
{
	unsigned char*	buf = NULL;
	int				buf_len = 0;
	unsigned char*	p = NULL;
	int				temp_len = 0;
	int				temp_len2 = 0;
	int				arr_ct = 0;
	int				left_len = 0;
	char			temp[4096] = { 0 };
	char			elem_val[4096] = { 0 };
	int				ct = 0;

	buf = (unsigned char*)Mem_Malloc(strlen(data) / 2 + 1);
	if (buf == NULL)
	{
		LOG_E("malloc buf error");
		return -1;
	}
	memset(buf, 0, sizeof(strlen(data) / 2 + 1));

	buf_len = STR_HEX2BIN(data, buf, strlen(data) / 2 + 1);

	//LOG_D("get array total len=[%#x]", buf_len);
	//14 字节的头
	if (buf_len <= 14)
	{
		LOG_E("get the varray len is too short,len=[%d]", buf_len);
		goto err;
	}

	p = buf;
	p += 14;
	temp_len = ((int)p[0]) * 256;
	temp_len += (int)p[1];
	//后移2个字节
	p += 2;

	//LOG_D("get array first len=[%#x]", temp_len);

	if (temp_len + 14 != buf_len)
	{
		LOG_E("the total data len is error, len=[%d]", temp_len);
		goto err;
	}

	
	if (*p != 0x09)
	{
		LOG_E("do not support the array [0x%02x]", *p);
		goto err;
	}

	//后移6个字节
	p += 6;
	temp_len2 = ((int)p[0]) * 256;
	temp_len2 += (int)p[1];

	//LOG_D("get array second len=[%#x]", temp_len2);

	if (temp_len2 + 16 != temp_len)
	{
		LOG_E("the second len is wrong, firetlen=[%d] seclen=[%d]", temp_len, temp_len2);
		goto err;
	}

	p += 8;
	if (*p != 0x88)
	{
		LOG_E("the start data flag is not 0x88 is[0x%02c]", *p);
		goto err;
	}

	p += 2;
	if ((int)*p == 0xFE)
	{
		p += 7;
	}
	else
	{
		p += 3;
	}

	//后续两个字节是数组个数 如果是0x00FE 则后续才表示数组个数
	arr_ct = ((int)p[0]) * 256;
	arr_ct += (int)p[1];

	if (arr_ct == 0x00FE)
	{
		p += 4;
		arr_ct = ((int)p[0]) * 256;
		arr_ct += (int)p[1];
	}

	LOG_D("get array ct=[%d]", arr_ct);
	
	p += 2;
	left_len = buf_len - (p-buf);
	//后续为LV格式的数据  L为1字节 如果是FE 则跳过3个字节 后续两个字节是长度
	while (left_len > 0)
	{
		//先解析长度
		temp_len = (int)p[0];
		if (temp_len == 0xFE)
		{
			p += 3;
			temp_len = ((int)p[0]) * 256;
			temp_len += (int)p[1];
			p += 2;
			left_len -= 5;
		}
		else
		{
			left_len -= 1;
			p += 1;
		}
		//LOG_D("get array elem len=[%d]", temp_len);

		//解析值
		STR_BIN2HEX(p, temp_len, 4096, temp);
		p += temp_len;
		left_len -= temp_len;
		//LOG_D("get array elem hex data[%s]", temp);

		memset(elem_val, 0, sizeof(elem_val));
		if (get_column_type_value(col->arr_type, temp, elem_val, col) < 0)
		{
			LOG_E("get_column_type_value arr error");
			goto err;
		}
		//LOG_D("get array elem act val[%s]", elem_val);
		if (ct == 0)
		{
			memset(val, 0, max_len);
		}
		strcat(val, elem_val);
		strcat(val, ",");
		ct++;
	}
	val[strlen(val) - 1] = '\0';		//去掉最后的逗号
	if (ct != arr_ct)
	{
		LOG_E("the array number is error, ct=[%d] act ct=[%d]", arr_ct, ct);
		goto err;
	}

	Mem_Free(buf);
	return 0;

err:
	Mem_Free(buf);
	return -1;
}


int get_column_type_value(int type, char* data, char* val, DICT_COL* col)
{
	char*	cval;

	switch (type)
	{
	case 2:		//number
		cval = (char*)Mem_Malloc(256);
		if (cval == NULL)
		{
			LOG_E("malloc cval error");
			return -1;
		}
		memset(cval, 0, 256);
		tt_number(data, cval);
		strcpy(val, cval);
		Mem_Free(cval);
		break;
	case 1:		//varchar2
		cval = (char*)Mem_Malloc(strlen(data) / 2 + 1);
		if (cval == NULL)
		{
			LOG_E("malloc cval error");
			return -1;
		}
		memset(cval, 0, strlen(data) / 2 + 1);
		tt_varchar2(data, cval, strlen(data) / 2 + 1);
		sprintf(val, "'%s'", cval);
		Mem_Free(cval);
		break;
	case 96:		//char
		cval = (char*)Mem_Malloc(strlen(data) / 2 + 1);
		if (cval == NULL)
		{
			LOG_E("malloc cval error");
			return -1;
		}
		memset(cval, 0, strlen(data) / 2 + 1);
		tt_char(data, cval, strlen(data) / 2 + 1);
		sprintf(val, "'%s'", cval);
		Mem_Free(cval);

		break;
	case 121:	//UDT NULL 标志
	case 122:
		sprintf(val, "%s", data);
		break;

	case 123:	//varray
		cval = (char*)Mem_Malloc(8192);
		if (cval == NULL)
		{
			LOG_E("malloc cval error");
			return -1;
		}
		memset(cval, 0, 8192);
		tt_varray(data, col, cval, 8192);
		sprintf(val, "%s", cval);
		Mem_Free(cval);
		break;
	default:
		sprintf(val, "'%s'", data);
	}

	return 0;
}


