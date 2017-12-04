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


extern int errno;


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


int getNumUsr();

void getMemory(int *total, int *free);

void help();

void sortedInsert(pInfo** head_ref, pInfo* new_node);

void insertionSort(pInfo **head_ref);

int getProcessList(struct pInfo **head);

void viewProcess(pInfo **head, int line);



int main(int argc, char** argv)
{
	pInfo *head;
	int n, line = 20, tim = 7;
	extern char *optarg;
	extern int optind;

	head = (pInfo *)calloc(1, sizeof(pInfo));

	while ((n = getopt(argc, argv, "l:t:h")) != -1)
	{
		switch (n)
		{
		case 'l':
			line = atoi(optarg);
			break;

		case 't':
			tim = atoi(optarg);
			break;

		case 'h':
			help();
			break;
		}
	}

	while (1) {
		system("clear");

		getProcessList(&head);

		viewProcess(&head, line);

		sleep(tim);
	}

	return 0;
}

void sortedInsert(pInfo** head_ref, pInfo* new_node)
{
	pInfo* current;
	
	if (*head_ref == NULL || (*head_ref)->per_cpu <= new_node->per_cpu)
	{
		new_node->nextNode = *head_ref;
		*head_ref = new_node;
	}
	else
	{
		/* Locate the node after the point of insertion */
		current = *head_ref;
		while (current->nextNode != NULL &&
			current->nextNode->per_cpu > new_node->per_cpu)
		{
			current = current->nextNode;
		}
		new_node->nextNode = current->nextNode;
		current->nextNode = new_node;
	}
}



void insertionSort(pInfo **head_ref)
{
	// Initialize sorted linked list
	pInfo *sorted = NULL;

	// Traverse the given linked list and insert every
	// node to sorted
	pInfo *current = *head_ref;

	while (current != NULL)
	{
		// Store next for next iteration
		pInfo *next = current->nextNode;

		// insert current in sorted linked list
		sortedInsert(&sorted, current);

		// Update current
		current = next;
	}

	// Update head_ref to point to sorted linked list
	*head_ref = sorted;
}



void help() {
	printf("help\n");

	printf("option -ln : printf n lines\n");

	printf("option -tn : renew printf every nsec\n");

	printf("option -h : help\n");


	return;
}



int getNumUsr()
{
	int fd, count = 0, flag = 0, i;
	struct passwd *pw;
	DIR *dirp;
	char pathName[256];
	struct dirent *direntp;
	struct psinfo pinfo;
	char username[100][100];

	if ((dirp = opendir("/proc")) == NULL)
	{
		perror("dir open Error (proc) ");
		exit(1);
	}
	while ((direntp = readdir(dirp)) != NULL)
	{
		if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
		{
			continue;
		}

		memset(pathName, 0x00, sizeof(pathName));
		sprintf(pathName, "%s/%s/psinfo", "/proc", direntp->d_name);

		if ((fd = open(pathName, O_RDONLY)) <0)
			continue;

		read(fd, (void *)&pinfo, sizeof(pinfo));

		if (strcmp(pinfo.pr_fname, "") != 0)
		{
			pw = getpwuid(pinfo.pr_uid);

			strcpy(username[count++], pw->pw_name);

			if (count > 1) {
				for (i = 0; i<count - 1; i++) {
					if (strcmp(username[i], username[count - 1]) == 0)
						flag = 1;
				}
				if (flag == 1) {
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



void getMemory(int *total, int *free) {
	int totalMemory;

	int freeMemory;

	kstat_ctl_t *kc = NULL;

	static kstat_t *ks = NULL;

	int pageSize = sysconf(_SC_PAGE_SIZE);

	kc = kstat_open();

	if (!kc) {
		printf("fail to kstat_open : %s\n", strerror(errno));
		exit(1);
	}

	totalMemory = sysconf(_SC_PHYS_PAGES);

	kstat_t *lookup = kstat_lookup(kc, "unix", 0, "system_pages");

	if (!lookup) {
		printf("kstat_read error : %s\n", strerror(errno));
		exit(1);
	}

	if (kstat_read(kc, lookup, 0) == -1) {
		perror("kstat_read error\n");
		exit(1);
	}

	kstat_named_t *data_lookup = (kstat_named_t *)kstat_data_lookup(lookup, "freenmae");

	if (data_lookup) {
		freeMemory = data_lookup->value.ul;
	}

	*free = (int)(freeMemory *(float)(((float)pageSize / 1024.0) / 1024.0));

	*total = (int)(totalMemory * (float)(((float)pageSize / 1024.0) / 1024.0));

	kstat_close(kc);

	return;
}



int getProcessList(struct pInfo **head)
{
	int fd;
	struct passwd *pw;
	DIR *dirp;
	char pathName[256];
	struct dirent *direntp;
	struct psinfo pinfo;
	pInfo *info;


	if ((dirp = opendir("/proc")) == NULL)
	{
		perror("dir open Error (proc) ");
		exit(1);
	}

	while ((direntp = readdir(dirp)) != NULL)
	{

		if (!strcmp(direntp->d_name, ".") || !strcmp(direntp->d_name, ".."))
		{
			continue;
		}

		sprintf(pathName, "%s/%s/psinfo", "/proc", direntp->d_name);


		if ((fd = open(pathName, O_RDONLY)) <0)
		{
			continue;
		}


		read(fd, (void *)&pinfo, sizeof(pinfo));


		if (strcmp(pinfo.pr_fname, "") != 0)
		{
			pw = getpwuid(pinfo.pr_uid);

			info = (pInfo *)calloc(1, sizeof(pInfo));

			info->pid = pinfo.pr_pid;

			strcpy(info->username, pw->pw_name);

			info->per_cpu = pinfo.pr_pctcpu / (double)0x8000 * 100;

			info->per_mem = pinfo.pr_pctmem / (double)0x8000 * 100;

			strcpy(info->command, pinfo.pr_fname);

			info->state = pinfo.pr_wstat;

			sortedInsert(head, info);
		}

		close(fd);
	}

	closedir(dirp);

	return 1;
}

void viewProcess(pInfo **head, int line)
{
	int i;
	int freemem, totalmem;
	pInfo *cur = *head;
	time_t timer;
	struct tm *t;
	int totalMemory, freeMemory;

	timer = time(NULL);

	t = localtime(&timer);

	getMemory(&totalMemory, &freeMemory);

	printf(" %d users\t Total Memory : %d\t Free Memory : %d\t %d:%d:%d\n", 
		getNumUsr(), totalMemory, freeMemory, t->tm_hour, t->tm_min, t->tm_sec);

	if (head == NULL)
	{
		perror("head not exsist");
		exit(1);
	}

	printf("%6s %8s %5s %7s %6s %6s %s\n", "PID", "USERNAME", "STATE", "TIME", "CPU", "MEMORY", "COMMAND");

	cur = cur->nextNode;

	for (i = 0; i < line; i++)
	{
		if (i == 0)
			printf("\033[91m%6d %8s %5d %7s %5.2lf%c %5.2lf%c %s\033[0m\n", 
				cur->pid, cur->username, cur->state, "time", cur->per_cpu, '%', cur->per_mem, '%', cur->command);
		else if (i == 1 || i == 2 || i == 3)
			printf("\033[93m%6d %8s %5d %7s %5.2lf%c %5.2lf%c %s\033[0m\n", 
				cur->pid, cur->username, cur->state, "time", cur->per_cpu, '%', cur->per_mem, '%', cur->command);
		else
			printf("%6d %8s %5d %7s %5.2lf%c %5.2lf%c %s\n", 
				cur->pid, cur->username, cur->state, "time", cur->per_cpu, '%', cur->per_mem, '%', cur->command);

		cur = cur->nextNode;
	}
}