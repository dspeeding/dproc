#include "basetype.h"

/*************************************************************************
@Purpose :	read file
@Param   :	filename		--file name
@Param   :	buffer			--data to buffer, it can be NULL
@Param   :	length			--file data len
@Return  :	0--success  -1--fail
@Modify  :
@Remark  :	if buffer is NULL, only return the file's length
*************************************************************************/
int CF_ReadFile(IN char* filename, OUT unsigned char* buffer, OUT unsigned long* length)
{
	unsigned long offset = 0;
	FILE *fp = NULL;

	fp = fopen(filename, "rb");
	if (NULL == fp)
	{
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	*length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (buffer != NULL)
	{
		while (offset < *length)
		{
			offset += fread(buffer + offset, 1, *length - offset, fp);
		}
	}
	fclose(fp);

	return 0;
}

/*************************************************************************
@Purpose :	get last error
@Param   :	None
@Return  :	errno
@Modify  :
@Remark  :	
*************************************************************************/
int CF_GetLastError(void)
{
#ifdef _WINDOWS
	return GetLastError();
#else
	return errno;
#endif
}

/*************************************************************************
@Purpose :	get current thread id
@Param   :	None
@Return  :	tid
@Modify  :
@Remark  :
*************************************************************************/
unsigned long CF_GetCurThreadId(void)
{
#ifdef _WINDOWS
	return (unsigned long)GetCurrentThreadId();
#else
	return (unsigned long)pthread_self();
#endif
}





