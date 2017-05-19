#include "basetype.h"

#ifndef _WINDOWS

#include <sys/vfs.h>
#include <signal.h>

typedef struct _TDEF_CPU_STAT_INFO
{
	char cpuname[20];
	long user;
	long nice;
	long sys;
	long idle;
	long iowait;
	long irq;
	long softirq;
}CPU_STAT_INFO;

//iostat -x
typedef struct _TDEF_DISK_IO_INFO
{
	char		dev_name[20];
	double		rrqm;
	double		wrqm;
	double		r;
	double		w;
	double		rsec;
	double		wsec;
	double		avgrq;
	double		avgqu;
	double		awaits;
	double		svctm;
	double		util;
}DISK_IO_INFO;


typedef void(*sighandler_t)(int);
int pox_system(char* cmd)
{
	int				ret = 0;
	sighandler_t	old_handler;

	old_handler = signal(SIGCHLD, SIG_DFL);
	ret = system(cmd);
	signal(SIGCHLD, old_handler);
	return ret;
}

//¶ÁÈ¡cpuÐÅÏ¢
static int get_cpu_stat_info(CPU_STAT_INFO* info)
{
	FILE*			fp;
	char			buf[1024] = { 0 };

	fp = fopen("/proc/stat", "r");
	if (fp == NULL)
	{
		LOG_E("read [/proc/stat] file error,[%d]", errno);
		return ERR_COMM_OPEN_FILE;
	}

	fgets(buf, sizeof(buf), fp);
	sscanf(buf, "%s%ld%ld%ld%ld%ld%ld%ld", info->cpuname, &info->user, &info->nice,
		&info->sys, &info->idle, &info->iowait, &info->irq, &info->softirq);

	fclose(fp);
	return ERR_OK;
}

/*************************************************************************
@Purpose :	get cpu usage
@Input   :	pCpuUsage	--[OUT]	cpu usage
@Return  :	0--success other--fail 
@Modify  :
@Remark  :
*************************************************************************/
int get_cpu_usage_info(double *pCpuUsage)
{
	int				nRet;
	long			all1, all2;
	CPU_STAT_INFO	a;
	CPU_STAT_INFO	b;
	double			usage;

	memset(&a, 0, sizeof(CPU_STAT_INFO));
	memset(&b, 0, sizeof(CPU_STAT_INFO));

	nRet = get_cpu_stat_info(&a);
	if (nRet != ERR_OK)
	{
		LOG_E("get_cpu_stat_info error[%#x]", );
		return nRet;
	}

	usleep(10000);
	nRet = get_cpu_stat_info(&b);
	if (nRet != ERR_OK)
	{
		LOG_E("get_cpu_stat_info 2 error[%#x]", );
		return nRet;
	}

	all1 = a.user + a.nice + a.sys + a.idle + a.iowait + a.irq + a.softirq;
	all2 = b.user + b.nice + b.sys + b.idle + b.iowait + b.irq + b.softirq;

	usage = (double)((all2 - all1 - (b.idle - a.idle))*1.0) / ((all2 - all1)*1.0) * 100;
	*pCpuUsage = usage;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	get mem info
@Input   :	pTotal	--[OUT]	mem total
			pUsage	--[OUT]	mem usage
@Return  :	0--success other--fail
@Modify  :
@Remark  :	return Kb
*************************************************************************/
int get_mem_info(long* pTotal, long* pUsage)
{
	FILE *	fp;
	char	szBuf[1024] = { 0 };
	char	szMemName[128] = { 0 };
	long	memNumTotal = 0;
	long	memNumFree = 0;
	char	szMemPer[128] = { 0 };

	fp = fopen("/proc/meminfo", "r");
	if (fp == NULL)
	{
		LOG_E("read [/proc/stat] file error, [%d]", errno);
		return ERR_COMM_OPEN_FILE;
	}

	fgets(szBuf, sizeof(szBuf), fp);

	sscanf(szBuf, "%s%ld%s", szMemName, &memNumTotal, szMemPer);

	fgets(szBuf, sizeof(szBuf), fp);
	sscanf(szBuf, "%s%ld%s", szMemName, &memNumFree, szMemPer);

	fclose(fp);

	*pTotal = memNumTotal;
	*pUsage = memNumTotal - memNumFree;

	return ERR_OK;
}

/*************************************************************************
@Purpose :	get mem info
@Input   :	pdAllSize	--[OUT]	disk total
			pdUseSize	--[OUT]	disk used
@Return  :	None
@Modify  :
@Remark  :	return Gb
*************************************************************************/
void get_disk_info(double* pdAllSize, double* pdUseSize)
{
	struct statfs	diskInfo;
	uint64_t		totalSize = 0;
	uint64_t		freeSize = 0;

	statfs("/", &diskInfo);

	totalSize = diskInfo.f_bsize * diskInfo.f_blocks;
	freeSize = diskInfo.f_bsize * diskInfo.f_bfree;
	*pdAllSize = totalSize*1.0 / 1024 / 1024 / 1024;
	*pdUseSize = *pdAllSize - freeSize*1.0 / 1024 / 1024 / 1024;
}

/*************************************************************************
@Purpose :	get disk IO info
@Input   :	pdIOUsage	--[OUT]	disk IO
@Return  :	0--success other--fail
@Modify  :
@Remark  :
*************************************************************************/
int get_disk_IO(double* pdIOUsage)
{
	FILE*			fp;
	int				nRet = 0;
	const char*		szFile = "/tmp/diskIO_tmp.txt";
	char			szCmd[1024] = { 0 };
	char			szBuf[1024] = { 0 };
	DISK_IO_INFO	ioInfo;

	memset(&ioInfo, 0, sizeof(DISK_IO_INFO));

	snprintf(szCmd, sizeof(szCmd), "iostat -x |grep sda > %s", szFile);
	nRet = pox_system(szCmd);
	if (nRet != 0)
	{
		LOG_E("cmd[%s] error[%d][%s]", szCmd, errno, strerror(errno));
		return ERR_COMM_SYS_CMD;
	}

	fp = fopen(szFile, "r");
	if (fp == NULL)
	{
		LOG_E("read [%s] file error[%d]", szFile, errno);
		return ERR_COMM_OPEN_FILE;
	}

	fgets(szBuf, sizeof(szBuf), fp);

	fclose(fp);

	sscanf(szBuf, "%s%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", ioInfo.dev_name,
		&ioInfo.rrqm, &ioInfo.wrqm, &ioInfo.r, &ioInfo.w, &ioInfo.rsec, &ioInfo.wsec,
		&ioInfo.avgrq, &ioInfo.avgqu, &ioInfo.awaits, &ioInfo.svctm, &ioInfo.util);

	*pdIOUsage = ioInfo.util;

	return ERR_OK;
}


static int get_network_IO_sub(uint64_t* pRecv, uint64_t* pSend)
{
	int		nRet = 0;
	char	szFile[260] = { 0 };
	char	szCmd[1024] = { 0 };
	FILE*	fp;
	char	szBuf[1024] = { 0 };

	char	szName[128] = { 0 };
	char	szRecv[128] = { 0 };
	char	szTran[128] = { 0 };


	sprintf(szFile, "/tmp/networkIO_Tmp.txt");
	sprintf(szCmd, "%s > %s", "cat /proc/net/dev |grep eth0|awk '{print$1,$2,$10}'", szFile);

	nRet = pox_system(szCmd);
	if (nRet != 0)
	{
		LOG_E("cmd[%s] error[%d][%s]", szCmd, errno, strerror(errno));
		return ERR_COMM_SYS_CMD;
	}

	fp = fopen(szFile, "r");
	if (fp == NULL)
	{
		LOG_E("read [%s] file error[%d]", szFile, errno);
		return ERR_COMM_OPEN_FILE;
	}

	fgets(szBuf, sizeof(szBuf), fp);

	fclose(fp);

	sscanf(szBuf, "%s%s%s", szName, szRecv, szTran);

	*pRecv = atoll(szRecv);
	*pSend = atoll(szTran);

	return ERR_OK;
}

/*************************************************************************
@Purpose :	get network info
@Input   :	dRecv	--[OUT]	network recv data
			dSend	--[OUT]	network send data
@Return  :	0--success other--fail
@Modify  :
@Remark  :  return Mb
*************************************************************************/
int get_network_IO(double* dRecv, double* dSend)
{
	int			nRet;
	uint64_t	recv1, recv2;
	uint64_t	send1, send2;

	nRet = get_network_IO_sub(&recv1, &send1);
	if (nRet != ERR_OK)
	{
		LOG_E("get_network_IO_sub 1 err[%#x]", nRet);
		return nRet;
	}

	sleep(1);


	nRet = get_network_IO_sub(&recv2, &send2);
	if (nRet != ERR_OK)
	{
		LOG_E("get_network_IO_sub 2 err[%#x]", nRet);
		return nRet;
	}

	*dRecv = (recv2 - recv1)*1.0 / 1024 / 1024;
	*dSend = (send2 - send1)*1.0 / 1024 / 1024;

	return ERR_OK;
}

#endif
