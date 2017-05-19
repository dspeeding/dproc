#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

#define ERR_OK						0x00000000	//OK
#define ERR_COMM_NULL_POINTER		0x00001001	//null pointer
#define ERR_COMM_OPEN_FILE			0x00001002	//open file error
#define ERR_COMM_MEM_MALLOC			0x00001003	//malloc error
#define ERR_COMM_SYS_CMD			0x00001004	//system cmd error
#define ERR_COMM_SYS_INTER			0x00001005	//internal error
#define ERR_COMM_OVER_MAX_CT		0x00001006	//over max count

#define ERR_SYS_ENV_NOT_SET			0x00002001	//env not set
#define ERR_SYS_XML_PARSE			0x00002002	//xml parse error
#define ERR_SYS_ODBC_CONN			0x00002003	//odbc conn error
#define ERR_SYS_ODBC_STMT			0x00002004	//odbc stmt sql error
#define ERR_SYS_ODBC_EXEC			0x00002005	//odbc exec sql error
#define ERR_SYS_ODBC_FETCH			0x00002006	//odbc fetch error
#define ERR_SYS_ODBC_BIND			0x00002007	//odbc bind var error

#define ERR_DB_NO_DATA				0x00004001	//no data in db
#define ERR_DB_DATA_INVALID			0x00004002	//data invalid


#endif // __ERROR_CODE_H__
