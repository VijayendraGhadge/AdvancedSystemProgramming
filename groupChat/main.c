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

pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER;     //statically initializing MUTEX
pthread_cond_t con=PTHREAD_COND_INITIALIZER;       //statically initializing condition variable

sem_t semaphore;
static char sem[10]="vijju";
//static int count=0;

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
	
	pthread_mutex_lock (&mut);    //Locking mutex
	
 /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */
    //fd = open(FILEPATH, O_RDWR | O_CREAT, (mode_t)0600);
    fd=shm_open(FILEPATH,O_CREAT|O_RDWR,(mode_t)0600);
    if (fd == -1) {
	perror("Error opening file for writing");
	exit(EXIT_FAILURE);
    }

  //  if (fstat(fd, &sb) == -1)           /* To obtain file size */
   //           perror("fstat");
//	size_t FILESIZE=sb.st_size;


    /* Stretch the file size to the size of the (mmapped) array of ints
     */
   /* res = lseek(fd, FILESIZE, SEEK_SET);
    if (res == -1) {
	close(fd);
	perror("Error calling lseek() to 'stretch' the file");
	exit(EXIT_FAILURE);
    }
    
    /* Something needs to be written at the end of the file to
     * have the file actually have the new size.
     * Just writing an empty string at the current file position will do.
     *
     * Note:
     *  - The current position in the file is at the end of the stretched 
     *    file due to the call to lseek().
     *  - An empty string is actually a single '\0' character, so a zero-byte
     *    will be written at the last byte of the file.
     */

    /* Now the file is ready to be mmapped.
     */
    if (fstat(fd, &sb) == -1)           /* To obtain file size */
               perror("fstat");
	size_t FILESIZE=sb.st_size;
	if(FILESIZE!=0)
	{
    map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }
    }
    /* Now write int's to the file as if it were memory (an array of ints).
     */
    char buffer[1000];
    int exit_cond=1;
    
    pthread_mutex_unlock (&mut);     //unlocking mutex.
    do
    {

    if(fgets(buffer,999,stdin)==NULL)
		{
			
			perror("\nscanning stdin fail\n");
			exit(0);
		}	
    exit_cond=strcmp(buffer,"exit\n");

    if(exit_cond!=0)
    	{	pthread_mutex_lock (&mut);    //Locking mutex
    		sem_post(&semaphore);
    		pid_t pid=getpid();
    		char line_to_write[1000];
    		//strncpy(line_to_write,itoa(pid,line_to_write,10));
    		//itoa(pid,line_to_write,10);
    		sprintf(line_to_write,"%d",pid);
    		strcat(line_to_write," ");
    		strcat(line_to_write,buffer);
    		//strcat(line_to_write,"\n");

    		res = write(fd, line_to_write, strlen(line_to_write));
    		if (res != strlen(line_to_write)) {
			close(fd);
			perror("Error writing last byte ie the empty char to the file");
			exit(EXIT_FAILURE);
    		}

    		pthread_cond_signal(&con);       //signalling waiting condition variables
    		pthread_mutex_unlock (&mut);     //unlocking mutex.
    		
    		sem_wait(&semaphore);

    	}
	}while(exit_cond!=0);



    /* Don't forget to free the mmapped memory
     */
	if (fstat(fd, &sb) == -1)           /* To obtain file size */
               perror("fstat");
	FILESIZE=sb.st_size;
    if (munmap(map, FILESIZE) == -1) {
	perror("Error un-mmapping the file");
	close(fd);
	exit(EXIT_FAILURE);
	/* Decide here whether to close(fd) and exit() or not. Depends... */
    }

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);
    shm_unlink(FILEPATH);
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
    	sem_wait(&semaphore);
	pthread_mutex_lock (&mut);    //Locking mutex	
    fd=shm_open(FILEPATH,O_RDWR,0);
    //fd = open(FILEPATH, O_RDWR | O_CREAT,(mode_t)0600);
    
    if (fd == -1) {
	//perror("Error opening file for reading");
	//exit(EXIT_FAILURE);
  	pthread_cond_wait(&con,&mut);    //waiting for condition varible.  	
  	pthread_mutex_unlock (&mut);     //unlocking mutex.
  	continue;
    }

    if (fstat(fd, &sb) == -1) perror("fstat");
	int FILESIZE=sb.st_size;
  	if(FILESIZE==0)
  	{
  	//	perror("file size is 0");
  	pthread_cond_wait(&con,&mut);    //waiting for condition varible.
  	pthread_mutex_unlock (&mut);     //unlocking mutex.
  	continue;
  	}
  	if (fstat(fd, &sb) == -1) perror("fstat");
  	FILESIZE=sb.st_size;
  	if (FILESIZE<=lines_read*sizeof(char))
  	{
  			//perror("shared mem has been read completely");
  	pthread_cond_wait(&con,&mut);    //waiting for condition varible.
  	pthread_mutex_unlock (&mut);     //unlocking mutex.
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
	//lines_read=lseek(fd,0,SEEK_CUR);
	//printf("\n%d\n", lines_read);
	//close(fd);
	pthread_mutex_unlock (&mut);     //unlocking mutex.
	sem_post(&semaphore);

    /*res = lseek(fd, lines_read, SEEK_SET);
    if (res == -1) {
	close(fd);
	perror("Error calling lseek() to 'stretch' the file in reader");
	exit(EXIT_FAILURE);
    }
    char temp[1000];
    int nread=0;
				while(((nread=read(fd,temp,1000))>0)&&(nread!=-1))
				{
					printf("%s", temp);
				}
				if(nread==-1)
				{
					close(fd);
					perror("Error reading sharedmem_file");
					exit(0);
				}
				if(nread==0)
				{
					lines_read=lseek(fd,0,SEEK_CUR);
					close(fd);
					printf("\nread was here and waiting");
					pthread_cond_wait(&con,&mut);    //waiting for condition varible.
					pthread_mutex_unlock (&mut);     //unlocking mutex.
					printf("\nread was here and now continuing");
				}
*/
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

	pthread_t read_th, write_th;
	int err;
	sem_open(sem,O_CREAT|O_RDWR);
	err=pthread_create(&write_th,NULL,&writer,NULL);
	if(err!=0)perror("Error creating producer thread");
	err=pthread_create(&read_th,NULL,&reader,NULL);
	if(err!=0)perror("Error creating consumer thread");
	pthread_join(read_th,NULL);
	pthread_join(write_th,NULL);
	sem_close(&semaphore);
	sem_unlink(sem);

	return EXIT_SUCCESS;
}
