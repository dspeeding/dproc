#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__


#define ERR_OK						0x00000000	//OK
#define ERR_COMM_NULL_POINTER		0x00001001	//null pointer
#define ERR_COMM_OPEN_FILE			0x00001002	//open file error
#define ERR_COMM_MEM_MALLOC			0x00001003	//malloc error
#define ERR_COMM_SYS_CMD			0x00001004	//system cmd error
#define ERR_COMM_PARAM				0x00001005	//parameter error
#define ERR_COMM_OUT_OF_INDEX		0x00001006	//out of index
#define ERR_COMM_NOT_FIND_DATA		0x00001007	//not find data


#define ERR_XODBC_ALLOC_HANDLE		0x00002001	//odbc alloc handle error
#define ERR_XODBC_SET_ATTR			0x00002002	//odbc set attribute error
#define ERR_XODBC_CONNECT			0x00002003	//odbc connect error
#define ERR_XODBC_EXECUTE			0x00002004	//odbc execute error
#define ERR_XODBC_BIND_COL			0x00002005	//odbc bind column error
#define ERR_XODBC_DEFINE_COL		0x00002006	//odbc define column error
#define	ERR_XODBC_NO_DATA			0x00002007	//odbc no data
#define ERR_XODBC_FETCH				0x00002008	//odbc fetch error
#define ERR_XODBC_ENDTRAN			0x00002009	//odbc end tran error

#endif // __ERROR_CODE_H__
