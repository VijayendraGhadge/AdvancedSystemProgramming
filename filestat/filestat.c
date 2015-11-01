#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>

void print_usage()
{
fprintf(stderr,"\n Usage: ./filestat [-toe]... [FILE]...\n");
}

int main (int argc, char * argv[])
{
int filetype=0,owner=0,env=0;
char opt;
extern int optind;
while((opt = getopt(argc, argv, "toe")) != -1) {	//while options exist in commandline getopt
  switch(opt) {
      case 't': filetype = 1;	break;
      case 'o':	owner = 1;	break;
      case 'e': env = 1;	break;
      default:
         print_usage();
         return -1;
   }
}

//printf("\nfiletype=%d\nowner=%d\nenv=%d",filetype,owner,env);
//printf("\n%d\n%d",argc,optind);

if (argc>optind+1)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nThis program takes only one command line argument\n");
print_usage();
return -1;
}

if (argc==optind)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nNo arguments provided, this program takes one command line argument\n");
print_usage();
return -1;
}

char * filename = argv[optind];

if (access(filename,F_OK)!=-1)
{
struct stat buff;
lstat(filename,&buff);   //lstat used for not following symlink to leaf bcz we need filetype and size of symlink //we generally would use stat or fstat according to application

	if(env==1)
	{
	extern char ** environ;
	char **var;
	int fd=open(filename,O_RDWR|O_APPEND);
		if (fd==-1)
		{
		fprintf(stderr,"Permission Denied\n");
		return -1;
		}
		else
		{
			for(var=environ;*var!=NULL;++var)
			{
			//int xx=write(fd,*var,strlen(*var));
			write(fd,*var,strlen(*var));
			write(fd,"\n",1);
			//printf("%s\n",*var);
			//printf("%d\n",xx);
			}
		}
	close(fd);
	}

	if(filetype==1)
	{
	int type=buff.st_mode;

	if (S_ISDIR (type) !=0)printf("File Type = Directory\n");
	if (S_ISCHR (type) !=0)printf("File Type = Character Special File\n");
	if (S_ISBLK (type) !=0)printf("File Type = Block Special File\n");
	if (S_ISREG (type) !=0)printf("File Type = Regular File\n");
	if (S_ISFIFO (type) !=0)printf("File Type = FIFO special pipe = PIPE\n");
	if (S_ISLNK (type) !=0)printf("File Type = Symbolic link\n");
	if (S_ISSOCK (type) !=0)printf("File Type = Socket\n");
	}

	if(owner==1)
	{
	int uid=buff.st_uid;
	struct passwd *pwd;
		if((pwd=getpwuid(uid))!=NULL)		//if pointer is not null username for uid does not exist
		{
		printf("Owner Name = %s\n",pwd->pw_name);
		}
		else
		{
		printf("Owner UID = %d\n",uid);
		}
	}

int size=buff.st_size;
printf("File size = %d\n",size);
}
else
{
fprintf(stderr,"\nFile %s does not exist \n",filename);
return -1;
}

return 0;
}
