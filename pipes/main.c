#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>

void print_usage(char * s)
{
fprintf(stderr,"\n Usage: %s [-h]... [pattern]...\n",s);
exit(EXIT_SUCCESS);
}

void print_help(char * s)
{
fprintf(stderr,"\n Usage: %s [-h]... [pattern]...\n\n",s);
fprintf(stderr,"\n The program %s is used to demonstrate use of pipes and to replicate the behaviour of 'ls | grep <pattern>' \n",s);

exit(EXIT_SUCCESS);
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

if (argc>2)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nThis program takes only one command line argument\n");
print_usage(argv[0]);
return EXIT_FAILURE;
}

if (argc==1)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nNo arguments provided, this program takes one command line argument\n");
print_usage(argv[0]);
return EXIT_FAILURE;
}

int fds[2];
pipe(fds);		//creating pipe
char *ar[80];
pid_t child;
child=fork();

switch(child)
{
	case -1:			//Error creating child
	perror("Error Creating Child");
	exit(0);
	break;

	case 0:				//child will do this
	close(fds[1]);		//closing write end of pipe (unused).
	close(0);			//closing stdin //current stdin
	dup(fds[0]);		//duplicating pipes read end with stdin of parent i.e setting read from pipe and not stdin
	//i.e make stdin come from read end of pipe
          //  execl("/bin/grep", "grep",argv[1], NULL);
	ar[0]="/bin/grep";
	ar[1]=argv[1];
	ar[2]=NULL;
	execvp(ar[0],ar);
//execv("/bin/grep",argv[1]);
	break;

	default:			//parent will do this
	close(fds[0]);		//closing read end of pipe (unused).
	close(1);			//closing current stdout
	dup(fds[1]);		//making stdout to go to write-end of pipe

//execl("/bin/ls","ls",NULL);
	ar[0] = "/bin/ls";
	ar[1] = NULL;
    execvp(ar[0],ar);
	break;
}

return EXIT_SUCCESS;
}
