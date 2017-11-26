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
typedef struct pInfo
{
	int pid;	              // 프로세스 id 를 저장하는 변수
	char username[255];   // 프로세스를 구동한 username 을 저장하는 변수
	double per_cpu;       // 프로세스의 CPU 점유율(%) 를 저장하는 변수
	double per_mem;      // 프로세스의 메모리 점유율(%) 을 저장하는 변수
	int time;	              // 프로세스 구동 시간을 저장하는 변수
	int state;	      // 프로세스의 상태를 저장하는 변수
	char command[255];    // 해당 프로세스의 명령어를 저장하는 변수 
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

	// /proc 디렉토리 오픈
	if((dirp = opendir("/proc")) == NULL)
	{
	    perror("dir open Error (proc) ");
	    exit(1);
	}
	// /proc 디렉토리에는 현재 프로세스들이 /pid 형태로 구성되어 있기 때문에 readdir 함수를                  사용해 검색한다.
	while((direntp = readdir(dirp)) != NULL)
	{
	    //현재 디렉토리, 이전 디렉토리는 제외하며 검색함.
	    if(!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
	    {
	        continue;
	    }
	    //psinfo 파일을 오픈하기 위한 사전작업
	    memset(pathName, 0x00, sizeof(pathName));
	    sprintf(pathName, "%s/%s/psinfo","/proc",direntp->d_name);

	    //psinfo 파일을 오픈함
	    if((fd = open(pathName, O_RDONLY)) <0 )
	    {
	        printf("denied = %s\n",pathName);
	        continue;
	    }

	    //오픈이 성공하면 pinfo 구조체에 복사
	    read(fd, (void *)&pinfo, sizeof(pinfo));
	
	    //구조체를 동적할당하여 값을 넣고 삽입정렬함
	    if(strcmp(pinfo.pr_fname,"") != 0)
	    {
	        //프로세스 username 을 가져오기 위한..

	        // 구조체에 값을 집어넣어 큐에 정렬시킨다.
	        info = (pInfo *)calloc(1,sizeof(pInfo));
	        info->pid = pinfo.pr_pid;
	        info->per_cpu = pinfo.pr_pctcpu/(double)0x8000 * 100;
	        info->per_mem = pinfo.pr_pctmem/(double)0x8000 * 100;
	        strcpy(info->command,pinfo.pr_fname);
	        info->state = pinfo.pr_wstat;

	        // 프로세스 구조체 삽입`
			printf("%d %lf %lf %s\n",info->pid, info->per_cpu,info->per_mem,info->command);
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
