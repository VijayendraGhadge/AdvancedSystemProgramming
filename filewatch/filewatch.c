#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include<sys/inotify.h>
#include <string.h>
#include<limits.h>
#include <dirent.h>

#define buffer_length (1024*((sizeof(struct inotify_event))+16))

void print_usage()
{
fprintf(stderr,"\n Usage: ./filewatch [Directory(optional)]\n");
}

void list_dir(char * dir)
{
DIR * dir_ptr=opendir(dir);		//directory pointer open directory.
struct dirent * entry;			//create pointer of directry-entry struct.
if(dir_ptr==NULL)			//if dir pointer is NULL 
{
perror("Could not open directory");
//return; dosent exit
exit(0);
}
while((entry=readdir(dir_ptr))!=NULL)	//while returns non NULL print files in dir
{
printf("%s\n",entry->d_name);
}
closedir(dir_ptr);
}

int check_file_in_dirlist(char * temp)
{

if(access(temp,F_OK)!=-1)
{
	if(is_file(temp)==1)
	{
	return 1;
	}
	else
	{
	fprintf(stderr,"%s is not a file",temp);
	return -1;
	}	
}
else
{
fprintf(stderr,"%s is not in the listed Directory",temp);
return -1;
}

return -1;
}

int is_file(char * file)
{
struct stat buff;
stat(file,&buff);
int mode=buff.st_mode;
if(S_ISREG(mode)!=0)return 1;
if(S_ISLNK(mode)!=0)return 1;

return -1;
}

int main (int argc, char * argv[])
{
char opt;
extern int optind;
while((opt = getopt(argc, argv, "h")) != -1) {	//while options exist in commandline getopt
  switch(opt) {
      case 'h': 
	fprintf(stderr,"\nProgram dosen't take options"); 
	print_usage();
	return -1;
      default:
         print_usage();
         return -1;
   }
}

if (argc>2)
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

char dir [PATH_MAX+1];

if (argc==1)
{
char buffer[PATH_MAX+1];
getcwd(buffer,PATH_MAX+1);
strcpy(dir,(char *)buffer);
}
else
{
char temp[100];
strcpy(dir,(char*)argv[1]);
realpath(dir,temp);
}

struct stat buff;
lstat(dir,&buff);
int mode=buff.st_mode;
if(S_ISDIR(mode)!=-1)
{
list_dir(dir);
}
else
{
fprintf(stderr,"%s is not a directory",dir);
return -1;
}

char file [PATH_MAX];
scanf("%s",file);
//fgets(file,sizeof(file),stdin);
//printf("\n%s......%s",dir,file);

int result=0;

char file_watch [PATH_MAX];
strcpy(file_watch,dir);
strcat(file_watch,"/");
strcat(file_watch,file);
result=check_file_in_dirlist(file_watch);

if(result==1)
{
//int x;
ssize_t x;
int fd=0,wd=0;
char buffer[buffer_length];
char *p;
fd=inotify_init();

if ( fd < 0 ) 
{
perror("inotify_init");
}

wd=inotify_add_watch(fd,file_watch,IN_OPEN|IN_MODIFY);

while(1)
{
x=read(fd,buffer,buffer_length);
 for (p=buffer;p<buffer+x;)
 {
	struct inotify_event* event=(struct inotify_event*)p;
	if(event->mask & IN_OPEN)printf("File Read\n");
	if(event->mask & IN_MODIFY)printf("File Modified\n");
	p+=sizeof(struct inotify_event) + event->len;
 }
}
inotify_rm_watch(fd,wd);
close(fd);
close(wd);

}
else
{
//printf("error");
return -1;
}

return 0;
}
