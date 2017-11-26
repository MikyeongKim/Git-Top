#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){

	int n;
	int rfd, wfd;
	char buf[10];

//	string file1,file2;


	printf("argc= %d \n",argc);
	for( n=0; n<argc; n++){
		printf("argv[%d] = %s \n", n , argv[n]);
	}

//	file1='argv[1]';
//	file2='argv[2]';

	rfd = open(argv[1],O_RDONLY);
	if(rfd == -1){
		perror("Open han.c");
		exit(1);
	}

	wfd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if( wfd == -1){
		perror("Open bit.c");
		exit(1);
	}

	while((n=read(rfd, buf, 8))>0)
		if( write(wfd, buf,n) !=n)
			perror("Write");

	if( n==-1) 
		perror("Read");

	close(rfd);
	close(wfd);

	return 0;

}
