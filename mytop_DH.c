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
#include <kstat.h>
#include <time.h>

int getNumUsr();
void getMemory(int *total, int *free);

extern int errno;

int main(){
	time_t timer;
	struct tm *t;
	int totalMemory, freeMemory;
	int usrCount;

	timer = time(NULL);

	t = localtime(&timer);

	getMemory(&totalMemory, &freeMemory);

	printf("%d users \t\t\t\t\t Total Memory : %d \t Free Memory : %d \t\t\t\t\t %d:%d:%d\n",getNumUsr(), totalMemory, freeMemory, t->tm_hour, t->tm_min, t->tm_sec);
}

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
					if(strcmp(username[i], username[count-1]) == 0)
						flag =1; 
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


void getMemory(int *total, int *free){
	int totalMemory;
	int freeMemory;
	kstat_ctl_t *kc = NULL;
	static kstat_t *ks = NULL;

	int pageSize = sysconf(_SC_PAGE_SIZE);
	kc = kstat_open();

	if(!kc){
		printf("fail to kstat_open : %s\n", strerror(errno));
		exit(1);
	}

	totalMemory = sysconf(_SC_PHYS_PAGES);
	kstat_t *lookup = kstat_lookup(kc, "unix", 0, "system_pages");

	if(!lookup){
		printf("kstat_read error : %s\n", strerror(errno));
		exit(1);
	}

	if(kstat_read(kc,lookup,0) == -1){
		perror("kstat_read error\n");
		exit(1);
	}

	kstat_named_t *data_lookup = (kstat_named_t *)kstat_data_lookup(lookup, "freenmae");

	if(data_lookup){
		freeMemory = data_lookup->value.ul;
	}

	*free = (int)(freeMemory *(float)(((float)pageSize/1024.0)/1024.0));
	*total = (int)(totalMemory * (float)(((float)pageSize/1024.0)/1024.0));

	kstat_close(kc);

	return;	
}
