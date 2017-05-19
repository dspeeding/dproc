#include "basetype.h"
#include "signal_proc.h"

extern int g_loop;

static void handle_proc_term(void)
{
	LOG_D("SIGTERM receive...");
	g_loop = 0;
}

static void handle_proc_cont(void)
{
	return;
}


static void handle_proc(int sig)
{
	switch (sig)
	{
	case SIGTERM:
		handle_proc_term();
		break;
	case SIGCONT:
		handle_proc_cont();
		break;
	default:
		LOG_D("receive sig[%d]", sig);
	}
}

static void resp_handle(int sig, siginfo_t* info, void* context)
{
	LOG_D("signal [%s] received", sig);
}

int init_signal(void)
{
	int			i;
	struct sigaction act;

	for (i = 0; i < 32; i++)
	{
		signal(i, SIG_IGN);
	}

	signal(SIGTERM, handle_proc);
	signal(SIGCONT, handle_proc);

	sigemptyset(&act.sa_mask);
	act.sa_sigaction = resp_handle;
	act.sa_flags = SA_SIGINFO;

	if (sigaction(CMD_SIGNAL, &act, NULL) < 0)
	{
		LOG_E("set command signal handle error[%s]", strerror(errno));
		return ERR_COMM_SYS_INTER;
	}

	return ERR_OK;
}

