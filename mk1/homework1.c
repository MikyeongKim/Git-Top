#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <curses.h>

typedef struct {
	int id;
	int status;
	pthread_t thread;
}Thread;

enum State{
	Idle,
	ReFresh=' ',
	Kill='k',
	Sort='o'
};

void IdleView();
void ReFreshView();
void KillView();
void SortView();

void View();

void ReadKey(void *object);
void SetState(int);

int cgetch(void);
int isRunning = 1;
enum State state=Idle;
enum State prevState=Idle;

int main(char* args){
//	initscr();
	Thread ReadKeyThread;
	Thread ViewThread;
	system("clear");

//	pthread_create(&(ReadKeyThread.thread),NULL,ReadKey,NULL);	
	pthread_create(&(ViewThread.thread),NULL,View,NULL);
//	pthread_join(ReadKeyThread.thread,(void *)&(ReadKeyThread.status));
	pthread_join(ViewThread.thread,(void *)&(ViewThread.status));
//	endwin();
}
void View(){
	while(isRunning){
		printf("asdasdasd");
	
		IdleView();
		sleep(1);	
	}
}
void ReadKey(void *object){
	int inputkey = 0;
	while(isRunning){
//		inputkey = cgetch();
//		SetState(inputkey);
	}
}
void SetState(int inputkey){
	switch(inputkey){
		case Idle :
			state = Idle;
			IdleView();
			break;
		case ReFresh :
			state = ReFresh;
			ReFreshView();
			break;
		case Kill :
			state = Kill;
			KillView();
			break;
		case Sort :
			state = Sort;
			SortView();
			break;
	}	
}

int cgetch(void){
	char c = 0;
	struct termios org_opts, new_opts;
	char res = 0;
	res = tcgetattr (STDIN_FILENO, &org_opts);
	
	memcpy(&new_opts,&org_opts,sizeof(new_opts));

	new_opts.c_lflag &= (~ICANON | ~ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	new_opts.c_lflag &= ~ICANON;
	new_opts.c_oflag =0;
	new_opts.c_iflag = IGNPAR | ICRNL;
	new_opts.c_cc[VMIN]=1;
	new_opts.c_cc[VTIME]=0;
	tcsetattr (STDIN_FILENO,TCSANOW,&new_opts);
	c = getchar();
	tcflush(stdin,TCIFLUSH);
	res = tcsetattr (STDIN_FILENO, TCSANOW, &org_opts);

	return c;
}

void IdleView(){

}
void ReFreshView(){

}
void KillView(){

}
void SortView(){

}

