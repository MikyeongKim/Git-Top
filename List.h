#define Data pInfo
#define strLen 15

typedef struct pInfo{
	unsigned int pid;		//PID
	char user[strLen];		//USERNAME
	double per_cpu;			//CPU
	double per_mem;			//MEMORY
	char commend[strLen];	//COMMAND

	//not yet
//	char pr[strLen];
//	char ni;
//	unsigned int virt;
//	unsigned int res;
//	unsigned int shr;
//	char s;
//	unsigned long time;
}PInfo;

typedef struct node{
	Data data;
	struct node* next;
}Node;

typedef struct list{
	Node* header;
	int length;
	void(*Add)(struct list*,Data);
	int(*Remove)(struct list*,Data);
	Data(*Search)(struct list*,unsigned int);
	void(*Print)(struct list*);
	void (*Sort)(struct list*);
}List;

List newList();
void Add(List*,Data);
int Remove(List*,Data);
Data SearchByPID(List*,unsigned int);
void Print(List*);
void Data_Print(Data data);
void Data_Sort_By_PerCpu(List *this);
int comp(const void * elem1, const void * elem2); 
