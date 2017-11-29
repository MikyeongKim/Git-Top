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


#define MAX_PROCESS_NAME 512
#define ESUCCESS	0
#define ENOPROC		1
#define ENONAME		2

typedef struct pInfo{
	int pid;
	char username[255];
	double per_cpu;
	double per_mem;
	int time;
	int state;
	char command[255];
	struct pInfo* nextNode;
}pInfo;


int getProcessNameByPid(int pid,char *name);


int getProcessList(void){
	int fd;
	struct passwd *pw;
	DIR *dirp;
	char pathName[256];
	struct dirent *direntp;
	struct psinfo pinfo;
	pInfo *info;

	char temp_username[255];
	//pid에서 유저네임 얻기위한 초기화
	int iPid =0;
	int ret = 0;
	char* pName=(char *)calloc(MAX_PROCESS_NAME, sizeof(char));

	if( (dirp = opendir("/proc"))==NULL)
	{
		perror("dir open Error (proc)");
		exit(1);

	}

	while(( direntp = readdir(dirp))!=NULL)
	{
		if( !strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name,".."))
			continue;

		memset(pathName,0x00,sizeof(pathName));
		sprintf(pathName, "%s/%s/psinfo","/proc",direntp->d_name);

		if( (fd=open(pathName, O_RDONLY))<0){
			printf("denied = %s \n",pathName);
			continue;
		}

		read( fd, (void *)&pinfo, sizeof(pinfo));

		if( strcmp(pinfo.pr_fname,"") != 0){

			info = (pInfo *)calloc(1,sizeof(pInfo));
			info->pid= pinfo.pr_pid;

//			temp_username = system("cat passwd | grep %s");
//			info->user_name = &temp_username;
			if(pName){
				iPid=atoi(info->pid);
			}else{
				printf("can't alloc\n");
				return 0;
			}
			ret=getProcessNameByPid(iPid,pName);
			switch(ret){
				case ESUCCESS:
					printf("success!!\n");
					info->user_name=pName;
					break;
				case ENOPROC:
					printf("isn't found");
					break;
				case ENONAME:
					printf("can't read cmdline");
					break;
				default:
					printf("unknown err");
					break;
			}

			info->per_cpu= pinfo.pr_pctcpu/(double)0x8000 * 100;
			info->per_mem= pinfo.pr_pctmem/(double)0x8000 * 100;
			strcpy(info->command, pinfo.pr_fname);
			info->state = pinfo.pr_wstat;

			printf("%d | %lf | %lf | %s\n",info->pid, info->per_cpu, info->per_mem, info->command);

//			temp_username = [];
		}

		close(fd);
	}

	closedir(dirp);

	return 1;

}


int getProcessNameByPid(int pid, char* name){
	int result = ESUCCESS;
	sprintf(name, "/proc/%d/cmdline",pid);
	FILE* fp = fopen(name,"r");
	if(!fp){
		result=ENOPROC;
	}
	else{
		size_t size;
		size=fread(name,sizeof(char),MAX_PROCESS_NAME,fp);
		if(size>0){
			name[size]='\0';
		}else{
			result = ENONAME;
		}
		fclose(fp);
	}
	return result;
}



int main(void){
	getProcessList();
	return 0;
}
