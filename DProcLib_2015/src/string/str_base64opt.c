#include "basetype.h"

/*Base64 enc/dec func*/
static char find_pos(char ch)
{
	int			i;
	char		cRet;
	char		Base64_Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	for (i = 0; i<64; i++)
	{
		if (Base64_Table[i] == ch)
		{
			cRet = (char)i;
			return cRet;
		}
	}
	return -1;
}

/*************************************************************************
@Purpose :	base64 encode
@Param   :	szInData			--	data
@Param   :	nInDataLen			--	data len
@Param   :	szOutData			--	base64 string
@Param   :	nOutDataMaxLen		--	max out buffer len
@Return  :	0--success  -1--fail
@Modify  :
@Remark  :
*************************************************************************/
int Base64_Encode(IN char* szInData, IN int nInDataLen, OUT char* szOutData, IN int nOutDataMaxLen)
{
	int prepare = 0;
	int ret_len;
	int temp = 0;
	int nOutLen = 0;
	char *f = NULL;
	int tmp = 0;
	char changed[4];
	int i = 0;
	const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";


	ret_len = nInDataLen / 3;
	temp = nInDataLen % 3;
	if (temp > 0)
	{
		ret_len += 1;
	}
	ret_len = ret_len * 4 + 1;

	memset(szOutData, 0, nOutDataMaxLen);
	f = szOutData;
	while (tmp < nInDataLen)
	{
		temp = 0;
		prepare = 0;
		memset(changed, '\0', 4);
		while (temp < 3)
		{
			//printf("tmp = %d\n", tmp); 
			if (tmp >= nInDataLen)
			{
				break;
			}
			prepare = ((prepare << 8) | (szInData[tmp] & 0xFF));
			tmp++;
			temp++;
		}
		prepare = (prepare << ((3 - temp) * 8));
		//printf("before for : temp = %d, prepare = %d\n", temp, prepare); 
		for (i = 0; i < 4; i++)
		{
			if (temp < i)
			{
				changed[i] = 0x40;
			}
			else
			{
				changed[i] = (prepare >> ((3 - i) * 6)) & 0x3F;
			}
			*f = base[(int)changed[i]];
			//printf("%.2X", changed[i]); 
			f++;
			nOutLen++;
			if (nOutLen >= nOutDataMaxLen)
			{
				return -1;
			}
		}
	}

	return 0;
}

/*************************************************************************
@Purpose :	base64 encode
@Param   :	szData				--	base64 string
@Param   :	nData				--	base64 string  len
@Param   :	bDataOut			--	decode buffer
@Param   :	nDataOutMaxLen		--	max decode buffer len
@Return  :	decode buffer len --success  -1--fail
@Modify  :
@Remark  :
*************************************************************************/
int Base64_Decode(IN char* szData, IN int nData, OUT unsigned char* bDataOut, IN int nDataOutMaxLen)
{
	int equal_count = 0;
	unsigned char* f = NULL;
	int tmp = 0;
	int temp = 0;
	int prepare = 0;
	int i = 0;
	int nOut = 0;
	int j;

	if (szData == NULL || bDataOut == NULL)
	{
		return -1;
	}
	memset(bDataOut, 0x00, nDataOutMaxLen);

	if (*(szData + nData - 1) == '=')
	{
		equal_count += 1;
	}
	if (*(szData + nData - 2) == '=')
	{
		equal_count += 1;
	}

	f = bDataOut;
	while (tmp < (nData - equal_count))
	{
		temp = 0;
		prepare = 0;
		while (temp < 4)
		{
			if (tmp >= (nData - equal_count))
			{
				break;
			}

			j = find_pos(szData[tmp]);

			if (j < 0)
			{
				return -1;
			}

			prepare = (prepare << 6) | (j);
			temp++;
			tmp++;
		}
		prepare = prepare << ((4 - temp) * 6);
		for (i = 0; i<3; i++)
		{
			if (i == temp - 1)
			{
				break;
			}
			*f = (char)((prepare >> ((2 - i) * 8)) & 0xFF);
			nOut++;
			f++;
			if (nOut >= nDataOutMaxLen)
			{
				return -1;
			}
		}
	}
	return nOut;
}


