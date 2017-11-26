WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes    
INCLUDE := -isystem /lib/modules/`uname -r`/build/include    
CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE}    
CC      := gcc                                 
TARGET  := my_proc 
     
my_proc.o: my_proc.c
	${CC} ${CFLAGS} -c my_proc.c 
	                                     
	.PHONY: clean                                                      
	                                          
	clean:                                    
		rm -rf ${TARGET}.o    
