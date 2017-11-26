#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <curses.h>
#include "List.h"

#define fsize 256

int getCmdLine(char* file,char* buf);

int main(int argc,char* args[]){
	DIR* dir; 
	struct dirent *entry; 
	struct stat fileStat;
	int pid = 0;
	char cmdLine[fsize];
	char tempPath[fsize];
	dir = opendir("/proc");

	while((entry=readdir(dir))!=NULL){
	
		lstat(entry->d_name,&fileStat);
	
		if(!S_ISDIR(fileStat.st_mode)) continue;
	
		pid = atoi(entry->d_name);
		if(pid<=0) continue;
		
		sprintf(tempPath,"/proc/%d/cmdline",pid);
		getCmdLine(tempPath,cmdLine);
		printf("[%d] %s\n", pid,cmdLine);
	
	}
	closedir(dir);
	return 0;
}


int getCmdLine(char* file,char*buf){
	FILE *srcFp = NULL;
	srcFp = fopen(file,"r");
	memset(buf,0,sizeof(buf));
	fgets(buf,sizeof(buf),srcFp);
	fclose(srcFp);

}

