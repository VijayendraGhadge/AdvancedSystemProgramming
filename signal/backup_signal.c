#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

clock_t start;
clock_t end;
time_t t1,t2;
int flag=0,count=0;

void print_usage()
{
fprintf(stderr,"\n Usage: ./signal\n");
}

static void stop_handler(int stop)
{
start=clock();
t1=time(NULL);
}

static void continue_handler(int cont)
{
end=clock();
t2=time(NULL);
flag=1;
}

int main (int argc, char * argv[])
{
signal(SIGTSTP,stop_handler);
signal(SIGCONT,continue_handler);
while(flag!=1)
{
pause();  //returns after receiving any signal.
//if(flag==1)
//count++;
}
//printf("Count%d",count);

//printf("\nCPU=%d",(int)start);
//printf("\nend=%d",(int)end);
//printf("\nt1=%ld",(long int)t1);
//printf("\nt2=%ld",(long int)t2);
printf("Real time between Stop and Continue Signal= %d Seconds\n",(int)(t2-t1));

printf("CPU time between Stop and Continue Signal= %G Seconds\n",((double)(end-start))/CLOCKS_PER_SEC);

return 0;
}

