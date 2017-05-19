#pragma once

#define LOG_LEV_ERROR	0x01
#define LOG_LEV_WARN	0x02
#define LOG_LEV_INFO	0x03
#define LOG_LEV_DEBUG	0x04
#define LOG_LEV_MORE	0x05

#define ERRLOG			"[ERROR]"
#define WARNLOG			"[WARN]"
#define INFOLOG			"[INFO]"
#define DEBUGLOG		"[DEBUG]"
#define MORELOG			"[MORE]"

#define MAX_FILE_PATH		260

int get_log_lev(char* basedir, int* plog_lev);

void LOG_Init(int nLogLevel, const char* szPrefix, const char* szLogFile);

void LOG_UnInit(void);

void xlog_error(const char* file, int line, const char* format, ...);

void xlog_debug1(const char* format, ...);

void xlog_debug2(const char* format, ...);

void xlog_info(const char* format, ...);
