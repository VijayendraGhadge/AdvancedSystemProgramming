#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/types.h>
#include <mqueue.h>

#define MNAME "/mq"

void handler (int sig_num) {
    printf ("Received sig %d.\n", sig_num);
}

void ex(void)
{
   mq_unlink(MNAME);
}

void print_usage(char * s)
{
fprintf(stderr,"\n Usage: %s [-h]...\n",s);
exit(EXIT_SUCCESS);
}

void print_help(char * s)
{
fprintf(stderr,"\n Usage: %s [-h]... \n\n",s);
fprintf(stderr,"\n The program %s is used to demonstrate use of posix message queue , and write on one thread and read on other\n",s);
fprintf(stderr, "\nTO EXIT THE PROGRAM TYPE in 'quit' \n");
exit(EXIT_SUCCESS);
}

void* writer(void* arg)
{
  struct mq_attr attr;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = 800;
	int res;
  mqd_t fd;
  int exit_cond=1;
printf("Message Queue:\n");
  do
  {
    char buffer[800];

    if(fgets(buffer,798,stdin)==NULL)
    {     
      perror("\nscanning stdin fail\n");
      exit(0);
    }
    exit_cond=strcmp(buffer,"quit\n");
    if(exit_cond!=0)
    { 
	   
      fd=mq_open(MNAME,O_WRONLY,0644,&attr);
      if (fd == (mqd_t)-1) 
      {
	     perror("Error opening queue for writing");
	     exit(EXIT_FAILURE);
      }
        res=mq_send(fd,buffer,strlen(buffer),0); /////////pronbably notify
        if(res==-1)
          {
            perror("Send error");
            exit(EXIT_FAILURE);
          }
          mq_close(fd);


    }
	}while(exit_cond!=0);

  exit(EXIT_SUCCESS);
	return NULL;
}

void* reader(void* arg)
{

	int res;
  mqd_t fd;
    
	  do
    { 
      fd=mq_open(MNAME,O_RDONLY);
     if (fd == (mqd_t)-1) 
      {
      perror("Queue opening in read failed");
       exit(EXIT_FAILURE);
      }
      char temp[16096];
    res=mq_receive(fd,temp,sizeof(temp),NULL);
    if(res>=0)
    {
    temp[res]='\0';
    printf("Received [%d] : %s\n",res,temp);
    }
  else
  {
    printf("%d\n", res);
    perror("receive fail:");
  }

mq_close(fd);
	}while(1);


	return NULL;
}

int main(int argc, char* argv[])
{

int help=0;
char opt;
extern int optind;
while((opt = getopt(argc, argv, "h")) != -1) {	//while options exist in commandline getopt
  switch(opt) {
      case 'h': help = 1;	break;
      default:
         print_usage(argv[0]);
         return EXIT_FAILURE;
   }
}

if(help==1)
{
	print_help(argv[0]);
}

if (argc>1)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nThis program takes no command line argument\n");
print_usage(argv[0]);
return EXIT_FAILURE;
}
  
  atexit(ex);
	pthread_t read_th, write_th;
	int err;
  mqd_t fd=mq_open(MNAME,O_CREAT|O_RDWR,(mode_t)0644,NULL);
  if(fd==(mqd_t)-1)
  {
    perror("mq_open failed");
    exit(EXIT_FAILURE);
  }
  else
  {
  close(fd);  
  }
  err=pthread_create(&read_th,NULL,&reader,NULL);
  if(err!=0)perror("Error creating consumer thread");
  err=pthread_create(&write_th,NULL,&writer,NULL);
	if(err!=0)perror("Error creating producer thread");
	
	pthread_join(read_th,NULL);
	pthread_join(write_th,NULL);

	return EXIT_SUCCESS;
}
