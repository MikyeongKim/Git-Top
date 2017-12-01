#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/times.h>
#include <limits.h>
#include <procfs.h>
#include <kstat.h>
#include <sys/kstat.h>

extern int errno;

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

int main(){
	int total =0 , free=0;

	getMemory(&total, &free);


	printf("total : %d, free : %d\n", total, free);
	

	return 0;
}

