#include <stdio.h>
#include <stdlib.h>
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


pid_t child;
child=fork();
int fds[1];
int writefs;
pipe(fds);
int status;

switch(child)
{
	case -1:			//Error creating child
	perror("Error Creating Child");
	exit(0);
	break;

	case 0:				//child will do this
//printf("\nChild process");
close(fds[1]);
//close(fds[1]); //close write side from parents
           // close(0); //close stdin
            dup2(0,fds[1]);
            execl("/bin/grep", "grep", (char *) argv[1]);
            perror("exec failed!");
            exit(20);
//execv("/bin/grep",argv[1]);
	break;

	default:			//parent will do this
//printf("\nParent process");
close(fds[0]);
dup2(1,fds[1]);

execl("/bin/ls","ls",NULL);
	break;

}

wait(&status);
	return EXIT_SUCCESS;
}
