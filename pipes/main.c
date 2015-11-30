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
int fileds[2];
pipe(fileds);
switch(child)
{
	case -1:			//Error creating child
	perror("Error Creating Child");
	exit(0);
	break;

	case 0:				//child will do this
printf("\nChild process");
//close(fileds[1]);
execv("/bin/grep",argv);
	break;

	default:			//parent will do this
printf("\nParent process");
//close(fileds[0]);
execv("/bin/ls",argv);
	break;

}


	return EXIT_SUCCESS;
}