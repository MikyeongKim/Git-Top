#include <stdio.h>

main(){
	setuid(0);
	setgid(0);
	system("/proc");
}
