///////////////////////////////////////////////////////////////////////////////
//ChildMonitor.c monitors child processes.									 //
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


clock_t start;
clock_t end;
time_t t1,t2;
volatile sig_atomic_t flag=0;

void print_usage()
{
fprintf(stderr,"\n Usage: ./childMonitor\n");
}

//void exitting()
//{

//}
static void child_term(int sig)
{
end=clock();
t2=time(NULL);
int old_errno=errno;
while(waitpid(-1,NULL,WNOHANG)>0)
	continue;
errno=old_errno;
flag=1;
}

/*
static void stop_handler(int stop)
{
t1=time(NULL);
done_stop=1;
}

static void continue_handler(int cont)
{
if(done_stop==1)
{
t2=time(NULL);

flag=1;
}
}
*/
int main (int argc, char * argv[])
{
pid_t child;
//on_exit(exitting,0);
int status;

struct sigaction mysig;
mysig.sa_handler=child_term;
//mysig.sa_flags=SA_RESTART;
sigaction(SIGCHLD,&mysig,NULL);

switch(child=fork())
{
	case -1:			//Error creating child
	perror("Error Creating Child");
	exit(0);
	break;
	case 0:				//child will do this
	while(1);
	break;
	default:			//parent will do this
		start=clock();
		t1=time(NULL);
		while(flag==0)
		{
			pause();
		}

//		signal(SIGTSTP,stop_handler);
//		signal(SIGCONT,continue_handler);

printf("\nReal time between Stop and Continue Signal= %d seconds\n",(int)(t2-t1));
printf("CPU time between Stop and Continue Signal= %Lf nanoseconds\n",(((long double)(end-start))/CLOCKS_PER_SEC)*1000000000);
	break;

}
return 0;
}

