#include "List.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



List newList() {
	List list;
	memset(&list, 0, sizeof(list));
	list.Add = Add;
	list.Remove = Remove;
	list.Search = SearchByPID;
	list.Print = Print;
	list.Sort = Data_Sort_By_PerCpu;
	list.length = 0;
	list.header = NULL;
	return list;
}

Node* newNode() {
	Node* node = (Node*)malloc(sizeof(Node));
	memset(node, 0, sizeof(node));
	node->next = NULL;

	return node;
}

void Add(List* this, Data data) {
	Node* currentNode = this->header;

	if (this->header == NULL) {
		this->header = newNode();
		this->header->data = data;
		this->length++;
		return;
	}

	while (currentNode->next != NULL) {
		currentNode = currentNode->next;
	}

	currentNode->next = newNode();
	currentNode->next->data = data;
	this->length++;
}

int Remove(List* this, Data data) {
	Node* currentNode = this->header;
	Node* temp = currentNode;
	int i = 0;

	if (currentNode == NULL) {
		printf("노드가 하나도 없쪙\n");
		return 0;
	}
	else if (currentNode->data.pid == data.pid) /// 해더가 가르키는 노드일때
	{
		temp = this->header;
		this->header = this->header->next;
		free(temp);
		this->length--;
		return 1;
	}
	else
	{
		for (i = 0; i < this->length - 1; i++)
		{

			if (currentNode->next->data.pid == data.pid)
			{
				temp = currentNode->next;
				currentNode->next = currentNode->next->next;
				free(temp);
				this->length--;
				return 1;
			}
		}

		return 0;//실패
	}
}



Data SearchByPID(List* this, unsigned int pid) {
	Node * node = this->header;
	Data data;
	memset(&data, 0, sizeof(data));
	int i = 0;
	if (this->length <= 0)
	{
		printf("찾는 데이터가 업서요\n");
		return data;
	}
	else
	{
		for (i = 0; i < this->length; i++)
		{
			if (node->data.pid == pid)
			{
				return node->data;
			}
			node = node->next;
		}
		printf("찾는 데이터가 업서요\n");
		return data;
	}

}

void Print(List *this) {
	int i = 0;
	Node *currentNode = this->header;
	if (this->length == 0) {
		printf("non existing Data\n");
		return;
	}

	for (i = 0; i < this->length; i++) {
		Data_Print(currentNode->data);
		currentNode = currentNode->next;
	}
}

void Data_Print(Data data)
{
	printf("%s\t", data.user); //char user[strLen];
	printf("%s\t", data.pr); //char pr[strLen];
	printf("%c\t", data.ni); //	char ni;
	printf("%d\t", data.pid); //unsigned int pid;
	printf("%u\t", data.virt); //unsigned int virt;
	printf("%u\t", data.res); //unsigned int res;
	printf("%u\t", data.shr); //unsigned int shr;
	printf("%c\t", data.s); //char s;
	printf("%u\t", data.per_cpu);//unsigned per_cpu;
	printf("%f\t", data.per_mem);//float per_mem;
	printf("%u\t", data.time);//unsigned long time;
	printf("%s\t", data.commend);//char commend[strLen];
	printf("\n");
}

void Data_Sort_By_PerCpu(List *this) // percpu 기준 정렬
{
	int cnt = 0;
	Node* loop = this->header;
	Data* resList = malloc(sizeof(Data)*this->length);
	for (cnt = 0; cnt < this->length; cnt++) {
		memcpy(&resList[cnt], &loop->data, sizeof(loop->data));
		loop = loop->next;
	}
	qsort(resList, this->length, sizeof(*resList), comp);
	loop = this->header;
	for (cnt = 0; cnt < this->length; cnt++) {
		memcpy(&loop->data, &resList[cnt], sizeof(loop->data));
		loop = loop->next;
	}
	free(resList);
}

int comp(const void * elem1, const void * elem2)
{
	Data* f = ((Data*)elem1);
	Data* s = ((Data*)elem2);
	return (int)(s->per_cpu - f->per_cpu);
}

int main() {
	List list = newList();
	Data test, test2, test3, return_Data;
	memset(&test, 0, sizeof(test));
	memset(&test2, 0, sizeof(test));
	memset(&test3, 0, sizeof(test));
	test.per_cpu = 4;
	test2.per_cpu = 7;
	test3.per_cpu = 3;
	test.pid = 1;
	test2.pid = 2;
	test3.pid = 2;


	list.Add(&list, test);
	list.Add(&list, test2);
	list.Add(&list, test3);
	list.Print(&list);
	list.Sort(&list);
	list.Print(&list);
	getchar();
}
