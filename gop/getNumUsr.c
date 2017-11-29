#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/times.h>
#include <limits.h>
#include <procfs.h>
#include <pwd.h>

int getNumUsr()
{
	int fd, count=0, flag=0, i;
	struct passwd *pw;
	DIR *dirp;
	char pathName[256];
	struct dirent *direntp;
	struct psinfo pinfo;
	char username[100][100];

	if((dirp = opendir("/proc")) == NULL)
	{
	    perror("dir open Error (proc) ");
	    exit(1);
	}
	while((direntp = readdir(dirp)) != NULL)
	{
	    if(!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
	    {
	        continue;
	    }

	    memset(pathName, 0x00, sizeof(pathName));
	    sprintf(pathName, "%s/%s/psinfo","/proc",direntp->d_name);

	    if((fd = open(pathName, O_RDONLY)) <0 )
	    {
	        printf("denied = %s\n",pathName);
	        continue;
	    }

	    read(fd, (void *)&pinfo, sizeof(pinfo));

	    if(strcmp(pinfo.pr_fname,"") != 0)
	    {
			pw = getpwuid(pinfo.pr_uid);

			strcpy(username[count++], pw->pw_name);

			if(count > 1){
				for(i=0; i<count-1; i++){
					if(strcmp(username[i], username[count-1]) == 0){
						flag =1; 
					}
				}
				if (flag == 1){
					count--; 
					flag = 0;
				}
			}
	    }

	    close(fd);
	}

	closedir(dirp);

	return count;
}

int main(void)
{
	int count;

	count = getNumUsr();

	printf("%d\n", count);

	return 0;
}
