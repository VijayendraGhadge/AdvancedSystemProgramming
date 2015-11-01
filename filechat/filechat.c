#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t flag=0;

void write_to_file(char *s,char * file)
{
	int fd=open(file,O_WRONLY|O_APPEND);
	if(fd==-1)
	{
		perror("Error opening output file");
		exit(0);
	}
	else
	{
	time_t t=time(NULL);
	char *temp=ctime(&t);
	temp[strlen(temp)-1]='\0';
	strcat(temp,",");
	strcat(temp,s);
	strcat(temp,"\n");
	write(fd,temp,strlen(temp));
	}
	close(fd);
}

void read_from_file(char * file)
{
	int fd=open(file,O_RDONLY);
		if(fd==-1)
		{
			perror("Error opening Input file");
			exit(0);
		}
		else
		{
			char temp[1000]={0};
			int nread=0;
			//printf("\n");
			while(((nread=read(fd,temp,1000))>0)&&(nread!=1))
			{
				printf("%s", temp);
			}
			if(nread==-1)
			{
				close(fd);
				perror("Error reading input file");
				exit(0);
			}
		}
		close(fd);
}


static void alarm_handle(int sig)
{
if(flag==1)
{
flag=0;
}
else
{
flag=1;
}
alarm(3);
}

void print_usage()
{
fprintf(stderr,"\nUsage: ./filechat [-f]... [INPUT FILE]... [OUTPUT FILE]...\n");
}

int main (int argc, char * argv[])
{
if (argc>3)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nThis program takes only two command line arguments\n");
print_usage();
return -1;
}

if (argc==1)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nNo arguments provided, this program takes two command line argument\n");
print_usage();
return -1;
}

if (argc==2)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nOnly one command line argument '%s' provided, this program takes two command line argument\n",argv[1]);
print_usage();
return -1;
}

struct sigaction mysig;
mysig.sa_handler=alarm_handle;
//mysig.sa_flags=SA_RESTART;
sigaction(SIGALRM,&mysig,NULL);

char in_file[100]={0};
strcat(in_file,argv[1]);
char out_file[100]={0};
strcat(out_file,argv[2]);
char buff [1000]={0};

while(1)
{
	alarm(3);
	if(flag==0)
	{
		//if(scanf("%999s",buff)!=1)
		if(fgets(buff,999,stdin)==NULL)
		{
			if(errno==EINTR)
			{
				continue;
			}
			else
			{
			perror("\nscanf fail\n");
			exit(0);
			}
		}	
		else
		{
		write_to_file(buff,out_file);
		}
	}
	else
	{
		read_from_file(in_file);
	}
	pause();
}
return 0;
}