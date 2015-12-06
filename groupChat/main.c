#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mman.h>

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
	string* shared_mem_name = (string*)arg;
	size_t i = 0;
	
	//push(queue, (void*)i, sizeof(i));
	
	return NULL;
}

void* read(void* arg)
{
	string* shared_mem_name = (string*)arg;
	
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
	string shared_mem_name="group_chat_shm";
	int err;

	err=pthread_create(&write_th,NULL,&write,&shared_mem_name);
	if(err!=0)perror("Error creating producer thread");
	err=pthread_create(&read_th,NULL,&read,&shared_mem_name);
	if(err!=0)perror("Error creating consumer thread");
	pthread_join(read_th,NULL);
	pthread_join(write_th,NULL);

	return EXIT_SUCCESS;
}
