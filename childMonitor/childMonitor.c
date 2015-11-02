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
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

time_t t1=0,t2=0;
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
	//printf("\n%d",siginfo->si_code);
	temp=print_error_code(siginfo->si_code);
	//clo=siginfo->si_utime + siginfo->si_stime;
	//printf("\nChild took:%Lf seconds",(((long double)(clo))/CLOCKS_PER_SEC));
		if (temp==1)
		{
			flag=1;	
			t2=time(NULL);
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
		t1=time(NULL);
		while(flag==0)
		{
			pause();
		}

	printf("\nChild took= %d seconds\n",(int)(t2-t1));
	break;

}
return 0;
}