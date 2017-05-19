#ifndef __COMM_FUNC_H__
#define __COMM_FUNC_H__


#ifdef __cplusplus
extern "C"
{
#endif

/*************************************************************************
@Purpose :	read file
@Param   :	filename		--file name
@Param   :	buffer			--data to buffer, it can be NULL
@Param   :	length			--file data len
@Return  :	0--success  -1--fail
@Modify  :
@Remark  :	if buffer is NULL, only return the file's length
*************************************************************************/
int CF_ReadFile(char* filename, unsigned char* buffer, unsigned long* length);

/*************************************************************************
@Purpose :	get last error
@Param   :	None
@Return  :	errno
@Modify  :
@Remark  :
*************************************************************************/
int CF_GetLastError(void);

/*************************************************************************
@Purpose :	get current thread id
@Param   :	None
@Return  :	tid
@Modify  :
@Remark  :
*************************************************************************/
unsigned long CF_GetCurThreadId(void);


#ifdef __cplusplus
}
#endif

#endif // __COMM_FUNC_H__




