#ifndef __LOG_H__
#define __LOG_H__

#define LOG_LEV_ERROR			0x01	
#define LOG_LEV_WARN			0x02	
#define LOG_LEV_INFO			0x03	
#define LOG_LEV_DEBUG			0x04		
#define LOG_LEV_MORE			0x05	

#define ERROLOG					"[ERROR] "
#define WARNLOG  				"[WARN] "
#define INFORLOG				"[INFO] "
#define DEBUGLOG				"[DEBUG] "
#define MORELOG					"[MORE] "


#define LOG_E(fmt, ...)							Log_E(NULL, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...)							Log_W(NULL, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...)							Log_I(NULL, fmt, ##__VA_ARGS__)
#define LOG_D(fmt, ...)							Log_D(NULL, fmt, ##__VA_ARGS__)
#define LOG_M(fmt, ...)							Log_M(NULL, fmt, ##__VA_ARGS__)

#define Log_E(modname, fmt, ...)				WriteTempLog(LOG_LEV_ERROR, modname, fmt, ##__VA_ARGS__)
#define Log_W(modname, fmt, ...)				WriteTempLog(LOG_LEV_WARN, modname, fmt, ##__VA_ARGS__)
#define Log_I(modname, fmt, ...)				WriteTempLog(LOG_LEV_INFO, modname, fmt, ##__VA_ARGS__)
#define Log_D(modname, fmt, ...)				WriteTempLog(LOG_LEV_DEBUG, modname, fmt, ##__VA_ARGS__)
#define Log_M(modname, fmt, ...)				WriteTempLog(LOG_LEV_MORE, modname, fmt, ##__VA_ARGS__)

#define LOGBIN_E(data, len)						LogBin_E(NULL, data, len)
#define LOGBIN_W(data, len)						LogBin_W(NULL, data, len)
#define LOGBIN_I(data, len)						LogBin_I(NULL, data, len)
#define LOGBIN_D(data, len)						LogBin_D(NULL, data, len)
#define LOGBIN_M(data, len)						LogBin_M(NULL, data, len)

#define LogBin_E(modname, data, len)			WriteTempBinLog(LOG_LEV_ERROR, modname, data, len)
#define LogBin_W(modname, data, len)			WriteTempBinLog(LOG_LEV_WARN, modname, data, len)
#define LogBin_I(modname, data, len)			WriteTempBinLog(LOG_LEV_INFO, modname, data, len)
#define LogBin_D(modname, data, len)			WriteTempBinLog(LOG_LEV_DEBUG, modname, data, len)
#define LogBin_M(modname, data, len)			WriteTempBinLog(LOG_LEV_MORE, modname, data, len)

#define WriteTempLog(nLevel, modname, fmt, ...)			WriteLog(nLevel, modname, fmt"\t[%s][%d]", ##__VA_ARGS__, __FILE__, __LINE__)

#define WriteTempBinLog(nLevel, modname, data, len)		WriteBinLog(nLevel, modname, data, len);

#define LOG_FUNC_START()	LOG_D("%s start...", __FUNCTION__)
#define LOG_FUNC_END()		LOG_D("%s end...", __FUNCTION__)

#define MAX_FILE_PATH		260

#ifdef __cplusplus
extern "C"
{
#endif
/*************************************************************************
@Purpose :	log init
@Param   :	nLogLevel	--	log level
@Param   :	szPrefix	--	prifix
@Param   :	szLogFile	--	log file
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void LOG_Init(IN int nLogLevel, IN const char* szPrefix, IN const char* szLogFile);

void LOG_UnInit(void);

/*************************************************************************
@Purpose :	write log
@Param   :	iLevel		-- log level
@Param   :	szModName	-- module name
@Param   :	format, ...	-- same as sprintf
@Return  :	None
@Modify  :
@Remark  :
*************************************************************************/
void WriteLog(IN int iLevel, IN const char* szModName, IN const char* format, ...);

/*************************************************************************
@Purpose :	write bin data log
@Param   :	iLevel		-- log level
@Param   :	bData		-- bin data
@Param   :	nDataLen	-- bin data len
@Return  :	None
@Remark  :
*************************************************************************/
void WriteBinLog(IN int iLevel, IN const char* szModName, IN unsigned char* bData, IN int nDataLen);

#ifdef __cplusplus
}
#endif

#endif // __LOG_H__

