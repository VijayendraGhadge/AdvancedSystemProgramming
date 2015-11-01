///////////////////////////////////////////////////////////////////////////////
// Read stdin until 3 seconds												//
// ver 1.0                                                                 									  //
// Language:    C, Linux 												    	  //
// Platform:    Dell XPS L520, Core i7, Windows 10, Linux VM           		  //
// Author:      Vijayendra Ghadge, Syracuse University	                   		  //
//              (315) 728 8953, vvghadge@syr.edu	                       					  //
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

volatile sig_atomic_t count=0;

void print_usage()
{
fprintf(stderr,"\n Usage: ./signal\n");
}

static void alarm_handle(int sig)
{
count++;
printf("\nTimeout %d\n",count);
if(count>=3)_exit(0);
alarm(3);
}

int main (int argc, char * argv[])
{
struct sigaction mysig;
mysig.sa_handler=alarm_handle;
mysig.sa_flags=SA_RESTART;
sigaction(SIGALRM,&mysig,NULL);

char buff [1000];
int x;
printf("\nType some text:\n");
while(count<=3)
{
alarm(3);
if(scanf("%999s",buff)!=1)
{
perror("\nscanf fail\n");
exit(0);
}
printf("\nDisplaying:%s\n",buff);
}

return 0;
}

