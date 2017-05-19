#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

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
int Base64_Encode(IN char* szInData, IN int nInDataLen, OUT char* szOutData, IN int nOutDataMaxLen);

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
int Base64_Decode(IN char* szData, IN int nData, OUT unsigned char* bDataOut, IN int nDataOutMaxLen);

#ifdef __cplusplus
}
#endif
