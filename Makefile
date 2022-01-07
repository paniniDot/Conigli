CFLAGS=-ansi -Wpedantic -Wall -Werror -D_THREAD_SAFE -D_REENTRANT -D_POSIX_C_SOURCE=200112L
LIBRARIES=-lpthread
LFLAGS=

all: conigli.exe

conigli.exe: conigli.o DBGpthread.o
	gcc ${LFLAGS} -o conigli.exe conigli.o DBGpthread.o ${LIBRARIES}

conigli.o: conigli.c DBGpthread.h
	gcc -c ${CFLAGS} conigli.c

DBGpthread.o: DBGpthread.c printerror.h
	gcc -c ${CFLAGS} DBGpthread.c

.PHONY: clean run

clean:
	rm -f *.exe *.o *~ core

run: conigli.exe
	./conigli.exe


