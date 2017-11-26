#define Data ProcessInfo
#define strLen 10

typedef struct processInfo{
	unsigned int pid;
	char user[strLen];
	char pr[strLen];
	char ni;
	unsigned int virt;
	unsigned int res;
	unsigned int shr;
	char s;
	unsigned per_cpu;
	float per_mem;
	unsigned long time;
	char commend[strLen];
}ProcessInfo;

typedef struct node{
	Data data;
	struct node* next;
}Node;

typedef struct list{
	Node* header;
	int length;
	void(*Add)(struct list*,Data);
	void(*Remove)(struct list*,Data);
	void(*Search)(struct list*,Data);
	void(*Print)(struct list*);
}List;

List newList();
void Add(List*,Data);
void Remove(List*,Data);
void Search(List*,Data);
void Print(List*);
