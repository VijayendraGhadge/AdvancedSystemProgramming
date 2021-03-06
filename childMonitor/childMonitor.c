///////////////////////////////////////////////////////////////////////////////
//ChildMonitor.c monitors child time and exit stat in parent process		 //
// ver 1.0                                                                 	 //
// Language:    C, Linux 												     //
// Platform:    Dell XPS L520, Core i7, Windows 10, Linux VM           		 //
// Author:      Vijayendra Ghadge, Syracuse University	                   	 //
//              (315) 728 8953, vvghadge@syr.edu	                       	 //
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <errno.h>

volatile sig_atomic_t flag=0;

void print_usage()
{
fprintf(stderr,"\n Usage: ./childMonitor\n");
}

int print_error_code(int err)
{
	switch(err)
	{
		case CLD_CONTINUED:
		printf("\nChild continued by SIGCONT");
		break;
		case CLD_DUMPED:
		printf("\nChild abnormal termination (core dump)");
		return 1;
		break;
		case CLD_EXITED:
		printf("\nChild exited");
		break;
		case CLD_KILLED:
		printf("\nChild abnormal termination (no core dump)");
		return 1;
		break;
		case CLD_STOPPED:
		printf("\nChild stopped");
		break;
		case CLD_TRAPPED:
		printf("\nTraced child stopped");
		break;
	}
	return 0;
}

static void child_term(int sig,siginfo_t * siginfo,void * context)
{
	int olderrno=errno;
	int temp=0;
	if(sig==SIGCHLD)
	{
	temp=print_error_code(siginfo->si_code);

		if (temp==1)
		{
			flag=1;	
		}
	}
	errno=olderrno;
}

int main (int argc, char * argv[])
{
pid_t child;
struct sigaction mysig;
//mysig.sa_handler=child_term;
mysig.sa_sigaction=(void*)child_term;
mysig.sa_flags=SA_RESTART|SA_SIGINFO;
sigaction(SIGCHLD,&mysig,NULL);
child=fork();
int status;
clock_t t=0;
//printf("\n%d",child);
switch(child)
{
	case -1:			//Error creating child
	perror("Error Creating Child");
	exit(0);
	break;

	case 0:				//child will do this
//	printf("\nChild process");
	while(1)
	{
		continue;
	}
	break;

	default:			//parent will do this
		//t1=time(NULL);
		while(flag==0)
		{
			wait(&status);
		}


	struct tms buff;
	times(&buff);
	t= buff.tms_cutime + buff.tms_cstime;

	printf("\nChild took= %lF seconds\n",((double)t)/sysconf(_SC_CLK_TCK));
	break;

}
return 0;
}