#include "List.h"
#include <stdio.h>

int main(){

}

List newList(){
	List list;
	memset(&list,0,sizeof(list));
	list.Add = Add;
	list.Remove = Remove;
	list.Search = Search;
	list.Print = Print;
	return list;
}

Node* newNode(){
	Node* node = malloc(sizeof(Node));
	memset(node,0,sizeof(node));
	node->next = NULL;
	return node;
}

void Add(List* this,Data data){
	Node* currentNode = this->header;
	if(this->header == NULL){
		this->header = newNode();
		this->header->data= data;
		this->length++;
		return;
	}
	while(currentNode->next != NULL){
		currentNode = currentNode->next;
	}
	currentNode->next = newNode();
	currentNode->next->data = data;
}

void Remove(List* this,Data data){

}

void Search(List* this,Data data){

}

void Print(List *this){

	Node *currentNode = this->header;
	if(this->header == NULL){
		printf("don't exsit Data\n");
		return;
	}
	while(currentNode != NULL){
	//	currentNode->data.Print(&currentNode->data);
		currentNode =currentNode->next;
	}	
}
