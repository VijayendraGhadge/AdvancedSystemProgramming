#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <sys/mman.h>
//#include <semaphore.h>
#include <pthread.h>
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
  struct stat sb;
  struct mq_attr attr;
	int res;
  mqd_t fd;
  int exit_cond=1;

  do
  {
    char buffer[1000];

    if(fgets(buffer,998,stdin)==NULL)
    {     
      perror("\nscanning stdin fail\n");
      exit(0);
    }
    exit_cond=strcmp(buffer,"quit\n");
    if(exit_cond!=0)
    { 
	   
      fd=mq_open(MNAME,O_WRONLY);
      if (fd == (mqd_t)-1) 
      {
	     perror("Error opening queue for writing");
	     exit(EXIT_FAILURE);
      }
      if(mq_getattr(fd,&attr)==-1)
      {
        perror("mq_getattr errored");
        exit(EXIT_FAILURE);
      }
      attr.mq_flags|=O_NONBLOCK;
      if (mq_setattr(fd,&attr,NULL)==-1)
      {
        perror("mq_setattr failed");
        exit(EXIT_FAILURE);
      }

      if (fstat(fd, &sb) == -1) perror("fstat");
/*      size_t FILESIZE=sb.st_size;
	       if(FILESIZE!=0)
	       { 
          map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
          if (map == MAP_FAILED) 
          {
          close(fd);
          perror("Error mmapping the file");
	        exit(EXIT_FAILURE);
          }
          res = lseek(fd, FILESIZE, SEEK_SET);
          if (res == -1) 
          {
          close(fd);
          perror("Error calling lseek() to 'stretch' the file");
          exit(EXIT_FAILURE);
          }
         }		
    		pid_t pid=getpid();
    		char line_to_write[1000];
    		sprintf(line_to_write,"%d",pid);
    		strcat(line_to_write," ");
    		strcat(line_to_write,buffer);
    		res = write(fd, line_to_write, strlen(line_to_write));
    		if (res != strlen(line_to_write)) 
        {
          close(fd);
          perror("Error writing last byte ie the empty char to the file");
          exit(EXIT_FAILURE);
    		}
    }*/
   // broadcast();
        printf("before write\n");
        res=mq_send(fd,buffer,strlen(buffer),1); /////////pronbably notify
        printf("%d\n", res);
        if(res==-1)
          {
            perror("Send error");
            exit(EXIT_FAILURE);
          }
          if(res==0)
          {
            printf("Successful write to mq\n");
          }
          close(fd);


    }
	}while(exit_cond!=0);

	
    
  exit(EXIT_SUCCESS);
	return NULL;
}
///////////////////////////////////////////////////////was here resume
void* reader(void* arg)
{

	int res;
  mqd_t fd;
   struct mq_attr attr;
	//int lines_read=0;
	//struct stat sb;

	  do
    { 
    fd=mq_open(MNAME,O_RDONLY);
    
     if (fd == (mqd_t)-1) 
      {
      perror("Queue opening in read failed");
       exit(EXIT_FAILURE);
      }
      if(mq_getattr(fd,&attr)==-1)
      {
        perror("mq_getattr errored");
        exit(EXIT_FAILURE);
      }
      attr.mq_flags|=O_NONBLOCK;
      if (mq_setattr(fd,&attr,NULL)==-1)
      {
        perror("mq_setattr failed");
        exit(EXIT_FAILURE);
      }

    char temp[1000];
    //strcpy(temp,map+lines_read);
	 //lines_read=strlen(map);
       
      sleep(5);
    res=mq_receive(fd,temp,998,NULL);
    printf("res=%d , %s\n",res, temp);
    sleep(2);
    if(res==0)
    {
      sleep(10);
      printf("\nwas here with res = %d",res);
    }
    else
    {
      
    }

    close(fd);
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
  
  struct sigevent sigevent;
  signal (handler);
  //sigevent.sigev_signo = SIGUSR1;
  atexit(ex);
	pthread_t read_th, write_th;
	int err;
  //count=sem_open(COUNT,O_CREAT|O_RDWR,(mode_t)0644,0);
  //sem_post(count);
	//wsem=sem_open(WSEM,O_CREAT|O_RDWR,(mode_t)0644,1);
  //rsem=sem_open(RSEM,O_CREAT|O_RDWR,(mode_t)0644,0);
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
