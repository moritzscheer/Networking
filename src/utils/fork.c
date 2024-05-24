
#include <err.h>

int fork(void)
{
	switch (fork())
	{
		case -1: err(EX_OSERR, "fork");
		case 0:
			// child process
			break;
		default:
			// parent processs
			break;
	}

}