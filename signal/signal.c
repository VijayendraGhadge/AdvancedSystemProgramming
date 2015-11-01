#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

clock_t start;
clock_t end;
time_t t1,t2;
int flag=0,count=0,done_stop=0;

void print_usage()
{
fprintf(stderr,"\n Usage: ./signal\n");
}

static void stop_handler(int stop)
{
t1=time(NULL);
start=clock();
done_stop=1;
}

static void continue_handler(int cont)
{
if(done_stop==1)
{
t2=time(NULL);
end=clock();
flag=1;
}
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
//printf("\nCPU=%Lf",(long double)start);
//printf("\nend=%Lf",(long double)end);
//printf("\nt1=%ld",(long int)t1);
//printf("\nt2=%ld",(long int)t2);
//printf("\nend=%Lf",(long double)CLOCKS_PER_SEC);
printf("\nReal time between Stop and Continue Signal= %d seconds\n",(int)(t2-t1));
printf("CPU time between Stop and Continue Signal= %Lf nanoseconds\n",(((long double)(end-start))/CLOCKS_PER_SEC)*1000000000);
return 0;
}

