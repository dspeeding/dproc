#ifndef __STR_UTILS_H__
#define __STR_UTILS_H__

#ifdef __cplusplus
extern "C"
{
#endif

//-----trim format-----
#define TRIM_LEFT			1
#define TRIM_RIGHT			2
#define TRIM_ALL			4


/*************************************************************************
@Purpose :	bin data to hex string
@Param   :	szOriData	--	bin data
@Param   :	nOriLen		--	bin data len
@Param   :	nDesMaxLen	--	target buffer max len
@Param   :	szDest		--	target buffer
@Return  :	0--success  -1--fail
@Modify  :
@Remark  :
*************************************************************************/
int STR_BIN2HEX(IN unsigned char* bOriData, IN int nOriLen, IN int nDesMaxLen, OUT char* szDest);

/*************************************************************************
@Purpose :	hex string to bin data
@Param   :	bufBCD		--	hex string
@Param   :	bufBIN		--	bin data buffer
@Param   :	nBinMaxLen	--	bin data buffer max len
@Return  :	bin data len--success  -1--fail
@Modify  :
@Remark  :
*************************************************************************/
int STR_HEX2BIN(IN const char* bufBCD, IN unsigned char* bufBIN, IN int nBinMaxLen);

/*************************************************************************
@Purpose :	reverse string
@Param   :	data			--	string
@Param   :	len				--	string len
@Return  :	void
@Modify  :
@Remark  :	abcd   ->   dcba
*************************************************************************/
void STR_Reverse(INANDOUT unsigned char *data, IN int len);

/*************************************************************************
@Purpose :	trim string
@Param   :	szSrc				--	string
@Param   :	nFlag				-- trim format
@Return  :	szSrc
@Modify  :
@Remark  :	nFlag
			1--TRIM_LEFT	Trim left
			2--TRIM_RIGHT	Trim right
			4--TRIM_ALL		Trim all
*************************************************************************/
char* STR_Trim(INANDOUT char* szSrc, IN int nFlag);

/*************************************************************************
@Purpose :	get current time to str
@Param   :	szTime		-- return time str
@Param   :	nMaxLen		-- max data buffer len
@Param   :	nFlag		-- format
@Return  :	szTime
@Modify  :
@Remark  :	nFlag
			1-- YYYY-MM-DD HH:mm:SS
			2-- YYYY/MM/DD HH:mm:SS
			3-- YYYYMMDDHHmmSS
*************************************************************************/
char* STR_GetCurTimeStr(OUT char* szTime, IN int nMaxLen, IN int nFlag);

/*************************************************************************
@Purpose :	get current str to time
@Param   :	szTime		-- return time str
@Param   :	nFlag		-- format
@Return  :	time_t
@Modify  :
@Remark  :	nFlag
			1-- YYYY-MM-DD HH:mm:SS
			2-- YYYY/MM/DD HH:mm:SS
			3-- YYYYMMDDHHmmSS
*************************************************************************/
time_t STR_Str2Time(IN char* szTime, IN int nFlag);


#ifdef __cplusplus
}
#endif
#endif // __STR_UTILS_H__

