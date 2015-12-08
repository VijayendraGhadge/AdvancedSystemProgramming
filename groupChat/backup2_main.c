#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>


#define FILEPATH "/group_chat_shm"
#define SEM "/sem"
#define RSEM "/rsem"
#define COUNT "/count"
//pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER;     //statically initializing MUTEX
//pthread_cond_t con=PTHREAD_COND_INITIALIZER;       //statically initializing condition variable

sem_t * semaphore;
sem_t * rsem;
sem_t * count;



void ex(void)
{
  int temp=100;  
  sem_wait(count);
  sem_getvalue(count,&temp);
  if(temp==0)
  {
    shm_unlink(FILEPATH);
    sem_unlink(RSEM);
    sem_unlink(SEM);
    sem_unlink(COUNT);
  }
}

void print_usage(char * s)
{
fprintf(stderr,"\n Usage: %s [-h]...\n",s);
exit(EXIT_SUCCESS);
}

void print_help(char * s)
{
fprintf(stderr,"\n Usage: %s [-h]... \n\n",s);
fprintf(stderr,"\n The program %s is used to communicate with different processes and simulate group chat between them\n",s);
fprintf(stderr, "\nTO EXIT THE PROGRAM TYPE in 'exit' \n");
exit(EXIT_SUCCESS);
}


void* writer(void* arg)
{
	int fd,res;
	char * map;
	struct stat sb;
  int exit_cond=1;
  do
  {
    char buffer[1000];

    if(fgets(buffer,999,stdin)==NULL)
    {
      
      perror("\nscanning stdin fail\n");
      exit(0);
    } 
    exit_cond=strcmp(buffer,"exit\n");
    if(exit_cond!=0)
    { 
	   
      fd=shm_open(FILEPATH,O_CREAT|O_RDWR,(mode_t)0600);
      if (fd == -1) {
	     perror("Error opening file for writing");
	     exit(EXIT_FAILURE);
      }

      if (fstat(fd, &sb) == -1) perror("fstat");
      size_t FILESIZE=sb.st_size;
	       if(FILESIZE!=0)
	       { 
          sem_wait(semaphore);
       // pthread_mutex_lock (&mut);    //Locking mutex
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
    		//strncpy(line_to_write,itoa(pid,line_to_write,10));
    		//itoa(pid,line_to_write,10);
    		sprintf(line_to_write,"%d",pid);
    		strcat(line_to_write," ");
    		strcat(line_to_write,buffer);
    		//strcat(line_to_write,"\n");
    		res = write(fd, line_to_write, strlen(line_to_write));
    		if (res != strlen(line_to_write)) 
        {
          close(fd);
          perror("Error writing last byte ie the empty char to the file");
          exit(EXIT_FAILURE);
    		}
    }
    		
        //pthread_cond_signal(&con);       //signalling waiting condition variables
    		//pthread_mutex_unlock (&mut);     //unlocking mutex.
   //     sem_post(rsem);
        sem_post(semaphore);
	}while(exit_cond!=0);



    /* Don't forget to free the mmapped memory
     */
	if (fstat(fd, &sb) == -1) perror("fstat");
	size_t FILESIZE=sb.st_size;
    if (munmap(map, FILESIZE) == -1) {
	perror("Error un-mmapping the file");
	close(fd);
	exit(EXIT_FAILURE);
    }
    close(fd);
    
    exit(EXIT_SUCCESS);

	return NULL;
}

void* reader(void* arg)
{
	
	int fd,res;
	int lines_read=0;
	char * map;
	struct stat sb;

	do
    {    
	//sem_wait(semaphore);
 //   sem_wait(rsem);
  //pthread_mutex_lock (&mut);    //Locking mutex	
    fd=shm_open(FILEPATH,O_RDWR,0);
    //fd = open(FILEPATH, O_RDWR | O_CREAT,(mode_t)0600);
    
    if (fd == -1) {
	//perror("Error opening file for reading");
	//exit(EXIT_FAILURE);
    //pthread_mutex_unlock (&mut);     //unlocking mutex.
      close(fd);
    //sem_post(semaphore);
     // sem_post(rsem);
    //pthread_cond_wait(&con,&mut);    //waiting for condition varible.  
      
  	continue;
    }

    if (fstat(fd, &sb) == -1) perror("fstat");
	  int FILESIZE=sb.st_size;
  	if(FILESIZE==0)
  	{
  	//	perror("file size is 0");
    //pthread_mutex_unlock (&mut);     //unlocking mutex.
      close(fd);
      //sem_post(rsem);
    //sem_post(semaphore);
    continue;
    //pthread_cond_wait(&con,&mut);    //waiting for condition varible.    	continue;
  	}
  	if (fstat(fd, &sb) == -1) perror("fstat");
  	FILESIZE=sb.st_size;
  	if (FILESIZE<=lines_read*sizeof(char))
  	{
  			//perror("shared mem has been read completely");
    //pthread_mutex_unlock (&mut);     //unlocking mutex.
    close(fd);
    //sem_post(rsem);
    //sem_post(semaphore);
    //pthread_cond_wait(&con,&mut);    //waiting for condition varible.     continue;
  	continue;
  	}
  	if (fstat(fd, &sb) == -1) perror("fstat");
  	FILESIZE=sb.st_size;
    map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
      close(fd);
      perror("Error mmapping the file for reading");
    	exit(EXIT_FAILURE);
      }
	
	printf("%s\n",map+lines_read);
	lines_read=strlen(map);
  close(fd); //////////////////////////////////////////////last change
	//lines_read=lseek(fd,0,SEEK_CUR);
	//printf("\n%d\n", lines_read);
	
  //sem_post(semaphore);
    //pthread_mutex_unlock (&mut);     //unlocking mutex.
    //sem_post(semaphore);
  //sem_post(rsem);
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
  count=sem_open(COUNT,O_CREAT|O_RDWR,(mode_t)0644,0);
  sem_post(count);
	semaphore=sem_open(SEM,O_CREAT|O_RDWR,(mode_t)0644,1);
  rsem=sem_open(RSEM,O_CREAT|O_RDWR,(mode_t)0644,0);
	err=pthread_create(&write_th,NULL,&writer,NULL);
	if(err!=0)perror("Error creating producer thread");
	err=pthread_create(&read_th,NULL,&reader,NULL);
	if(err!=0)perror("Error creating consumer thread");
	pthread_join(read_th,NULL);
	pthread_join(write_th,NULL);

	return EXIT_SUCCESS;
}
