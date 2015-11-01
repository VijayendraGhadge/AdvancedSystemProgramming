#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <utime.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>

void print_usage()
{
fprintf(stderr,"\n Usage: ./fileclone [-f]... [SOURCE FILE]... [DESTINATION FILE/PATH]...\n");
}

int clone_file(char * src_file,char * dst_file, int force)
{
if (access(src_file,F_OK)!=-1)
{
struct stat buff;
//lstat(src_file,&buff);   //lstat used for not following symlink to leaf bcz we need filetype and size of symlink //we generally would use stat or fstat according to application

int fd_src=open(src_file,O_RDONLY);
	if (fd_src==-1)
	{
	fprintf(stderr,"cannot open %s for reading: Permission Denied\n",src_file);
	return -1;
	}
	else
	{
		lstat(dst_file,&buff);
		int type=buff.st_mode;
			if (S_ISDIR (type) !=0)   //check if it is a directory 
			{
				if(access(dst_file,F_OK)!=-1) //check if it exists 
				{
					if(access(dst_file,W_OK)!=-1) //if write permissions then use name of source file.
					{
					dst_file=strcat(dst_file,"/");
					dst_file=strcat(dst_file,(char *)basename(src_file));
						if(access(dst_file,F_OK)!=-1)
						{
							if(access(dst_file,W_OK)!=-1)
							{
								if(force==1)
								{
								int fd_dst=open(dst_file,O_WRONLY);
									if (fd_dst!=-1)
									{
									int nread=0;
									char str [1024];
									while((nread=read(fd_src,str,1024))!=0)
									{
									write(fd_dst,str,nread); //writeToFile
									}
									close(fd_src);
									close(fd_dst);
									return 1;
									}
									else
									{
									perror("Error opening destination file");
									return -1;
									}
								}
								else
								{
								fprintf(stderr,"File already exists %s\n",dst_file);
								return -1;
								}
							}
							else
							{
							fprintf(stderr,"Cannot open %s for writing: Permission Denied\n",dst_file);
							return -1;					
							}
						}
						else
						{//create and write to a file
						int fd_dst=open(dst_file,O_WRONLY|O_CREAT);
							if (fd_dst!=-1)
							{
							int nread=0;
							char str [1024];
								while((nread=read(fd_src,str,1024))!=0)
								{
								write(fd_dst,str,nread);
								}
							close(fd_src);
							close(fd_dst);
							return 1;
							}
							else
							{
							perror("Error opening destination file");
							return -1;
							}
						}					
					}
					else
					{
					fprintf(stderr,"Cannot open %s for writing: Permission Denied\n",dst_file);
					return -1;					
					}
				}
				else
				{
				fprintf(stderr,"\nDirectory %s does not exist \n",dst_file);
				return -1;
				}				
			}
			if(access(dst_file,F_OK)!=-1)
			{
				if(access(dst_file,W_OK)!=-1)
				{
					if(force==1)
					{
					int fd_dst=open(dst_file,O_WRONLY);
						if (fd_dst!=-1)
						{
						int nread=0;
						char str [1024];
							while((nread=read(fd_src,str,1024))!=0)
							{
							write(fd_dst,str,nread);
							}
						close(fd_src);
						close(fd_dst);
						return 1;
						}
						else
						{
						perror("Error opening destination file");
						return -1;
						}
					}
					else
					{
					fprintf(stderr,"File already exists %s\n",dst_file);
					return -1;
					}
				}
				else
				{
				fprintf(stderr,"Cannot open %s for writing: Permission Denied\n",dst_file);
				return -1;					
				}
			}
			else
			{
			int fd_dst=open(dst_file,O_WRONLY|O_CREAT);
				if (fd_dst!=-1)
				{
					int nread=0;
					char str [1024];
					while((nread=read(fd_src,str,1024))!=0)
					{
					write(fd_dst,str,nread);
					}
					close(fd_src);
					close(fd_dst);
				return 1;
				}
				else
				{
				perror("Error opening destination file");
				return -1;
				}
			}
	}	
}
else
{
fprintf(stderr,"\nFile %s does not exist \n",src_file);
return -1;
}

}

int clone_file_params(char * src_file,char * dst_file)
{
struct stat buff;
lstat(src_file,&buff);
int mode=buff.st_mode;		//copy permission bits
int result=chmod(dst_file,mode);
if (result!=0)
{
perror("Permission Bits could not be set");
return -1;
}
int uid=buff.st_uid;		//copy uid,gid
int gid=buff.st_gid;
result=chown(dst_file,uid,gid);
if (result!=0)
{
perror("Owner could not be set for Destionation file");
return -1;
}

struct utimbuf time;  //in utime.h
time.actime=buff.st_atim.tv_sec;  //copy time_t of access time ,similarly modified time
time.modtime=buff.st_mtim.tv_sec;
result=utime(dst_file,&time);
if (result!=0)
{
perror("Time could not be set for Destionation file");
return -1;
}

return 1;
}

int main (int argc, char * argv[])
{
int force=0;
char opt;
extern int optind;
while((opt = getopt(argc, argv, "f")) != -1) {	//while options exist in commandline getopt
  switch(opt) {
      case 'f': force = 1;	break;
      default:
         print_usage();
         return -1;
   }
}

if (argc>optind+2)
{
int i;
	for(i=0;i<argc;++i)
	{
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\nThis program takes only two command line arguments\n");
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
fprintf(stderr,"\nNo arguments provided, this program takes two command line argument\n");
print_usage();
return -1;
}

char * src_file = argv[optind];
char * dst_file = argv[optind+1];

int cloned=0;
cloned=clone_file(src_file,dst_file,force); //create a cloned file

if (cloned==1)
{
int success=clone_file_params(src_file,dst_file); //clone original file parameters
if (success==1)return 1;
}


return 0;
}
