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

typedef struct pInfo
{
	int pid;	           
	char username[255];
	double per_cpu;  
	double per_mem;
	int time;	           
	int state;	   
	char command[255];  
	struct pInfo* nextNode;
}pInfo;

int getProcessList(void)
{
	int fd;
	struct passwd *pw;
	DIR *dirp;
	char pathName[256];
	struct dirent *direntp;
	struct psinfo pinfo;
	pInfo *info;

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
	        info = (pInfo *)calloc(1,sizeof(pInfo));
	        info->pid = pinfo.pr_pid;
			strcpy(info->username, pw->pw_name);
	        info->per_cpu = pinfo.pr_pctcpu/(double)0x8000 * 100;
	        info->per_mem = pinfo.pr_pctmem/(double)0x8000 * 100;
	        strcpy(info->command,pinfo.pr_fname);
	        info->state = pinfo.pr_wstat;

			printf("%5d %-10s %lf %lf %s\n",info->pid, info->username,  info->per_cpu,info->per_mem,info->command);
	    }

	    close(fd);
	}

	closedir(dirp);

	return 1;
}

int main(void)
{
	getProcessList();

	return 0;
}
