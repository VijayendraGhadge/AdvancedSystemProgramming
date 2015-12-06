#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mman.h>

#define FILEPATH "/tmp/group_chat_shm";

static int count=0;

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


void* write(void* arg)
{
	int fd,res;
	char * map;

	struct stat sb;


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

    if (fstat(fd, &sb) == -1)           /* To obtain file size */
               perror("fstat");
	size_t FILESIZE=sb.st_size;


    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    res = lseek(fd, FILESIZE-1, SEEK_SET);
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
    res = write(fd, "", 1);
    if (result != 1) {
	close(fd);
	perror("Error writing last byte ie the empty char to the file");
	exit(EXIT_FAILURE);
    }

    /* Now the file is ready to be mmapped.
     */
    map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }
    
    /* Now write int's to the file as if it were memory (an array of ints).
     */
    char *buffer;
    size_t bufsize = 1000;
    size_t characters;
    int exit_cond=1;
    buffer = (char *)malloc(bufsize * sizeof(char));
    /*if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }*/
    
    do
    {
    characters = getline(&buffer,&bufsize,stdin);
    //printf("%zu characters were read.\n",characters);
    //printf("You typed: '%s'\n",buffer);
    exit_cond=strcmp(buffer,"exit");
    if(exit_cond!=0)
    	{	
    		pid_t pid=getpid();
    		char*line_to_write;
    		//strncpy(line_to_write,itoa(pid,line_to_write,10));
    		itoa(pid,line_to_write,10);
    		strcat(line_to_write,buffer);
    		strcpy(map,line_to_write);
    	}
	}while(exit_cond!=0)



    /* Don't forget to free the mmapped memory
     */
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

void* read(void* arg)
{
	
	int fd,res;
	int lines_read=0;
	char * map;

 /* Open a file for writing.
     *  - Creating the file if it doesn't exist.
     *  - Truncating it to 0 size if it already exists. (not really needed)
     *
     * Note: "O_WRONLY" mode is not sufficient when mmaping.
     */
    //fd = open(FILEPATH, O_RDWR | O_CREAT, (mode_t)0600);
    fd=shm_open(FILEPATH,O_CREAT|O_RDONLY,(mode_t)0600);
    if (fd == -1) {
	perror("Error opening file for writing");
	exit(EXIT_FAILURE);
    }

    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    res = lseek(fd, FILESIZE-1, SEEK_SET);
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
    /*
    map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
	close(fd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }
    /* Now write int's to the file as if it were memory (an array of ints).
     */
    char *buffer;
    size_t bufsize = 32;
    size_t characters;
    int exit_cond=1;
    buffer = (char *)malloc(bufsize * sizeof(char));
    /*if( buffer == NULL)
    {
        perror("Unable to allocate buffer");
        exit(1);
    }*/
    if(res>lines_read)
    {

    }
    do
    {
    characters = getline(&buffer,&bufsize,stdin);
    //printf("%zu characters were read.\n",characters);
    //printf("You typed: '%s'\n",buffer);
    exit_cond=strcmp(buffer,"exit");
    if(exit_cond!=0)
    	{	
    		pid_t pid=getpid();
    		char*line_to_write;
    		strncpy(line_to_write,itoa(pid,line_to_write,10));
    		strcat(line_to_write,buffer);
    		strcpy(map,line_to_write);
    	}
	}while(exit_cond!=0)



    /* Don't forget to free the mmapped memory
     *//*
    if (munmap(map, FILESIZE) == -1) {
	perror("Error un-mmapping the file");
	close(fd);
	exit(EXIT_FAILURE);
    }*/
    	/* Decide here whether to close(fd) and exit() or not. Depends... */

    /* Un-mmaping doesn't close the file, so we still need to do that.
     */
    close(fd);
    exit(EXIT_SUCCESS);


	return NULL;
}

bool increment_count()
{

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

	err=pthread_create(&write_th,NULL,&write,NULL);
	if(err!=0)perror("Error creating producer thread");
	err=pthread_create(&read_th,NULL,&read,NULL);
	if(err!=0)perror("Error creating consumer thread");
	pthread_join(read_th,NULL);
	pthread_join(write_th,NULL);

	return EXIT_SUCCESS;
}
