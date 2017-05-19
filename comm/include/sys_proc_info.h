#ifndef __SYS_PROC_INFO_H__
#define __SYS_PROC_INFO_H__


#endif // __SYS_PROC_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _WINDOWS

/*************************************************************************
@Purpose :	get cpu usage
@Input   :	pCpuUsage	--[OUT]	cpu usage
@Return  :	0--success other--fail
@Modify  :
@Remark  :
*************************************************************************/
int get_cpu_usage_info(double *pCpuUsage);

/*************************************************************************
@Purpose :	get mem info
@Input   :	pTotal	--[OUT]	mem total
			pUsage	--[OUT]	mem usage
@Return  :	0--success other--fail
@Modify  :
@Remark  :	return Kb
*************************************************************************/
int get_mem_info(long* pTotal, long* pUsage);

/*************************************************************************
@Purpose :	get mem info
@Input   :	pdAllSize	--[OUT]	disk total
			pdUseSize	--[OUT]	disk used
@Return  :	None
@Modify  :
@Remark  :	return Gb
*************************************************************************/
void get_disk_info(double* pdAllSize, double* pdUseSize);

/*************************************************************************
@Purpose :	get disk IO info
@Input   :	pdIOUsage	--[OUT]	disk IO
@Return  :	0--success other--fail
@Modify  :
@Remark  :
*************************************************************************/
int get_disk_IO(double* pdIOUsage);

/*************************************************************************
@Purpose :	get network info
@Input   :	dRecv	--[OUT]	network recv data
			dSend	--[OUT]	network send data
@Return  :	0--success other--fail
@Modify  :
@Remark  :  return Mb
*************************************************************************/
int get_network_IO(double* dRecv, double* dSend);

#endif

#ifdef __cplusplus
}
#endif

