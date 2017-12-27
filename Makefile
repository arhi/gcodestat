CC     := gcc
CFLAGS := -Wall -Werror -IE:\Dev\eclipse-workspace\curl\include 
LFLAGS := -LE:\Dev\eclipse-workspace\curl\lib
LIBS   := -lm -lcurldll

SRCS   := gcodestat.c calcmove.c readconfig.c readgcode.c
HFILES := gcodestat.h calcmove.h readconfig.h readgcode.h

OBJS   := ${SRCS:.c=.o} 
PROGS  := gcodestat.exe

.PHONY: all
all: ${PROGS}

${PROGS}: ${OBJS} Makefile
	${CC} ${OBJS} ${LFLAGS} ${LIBS} -g -o $@ 

clean:
	rm -f ${PROGS} ${OBJS}

%.o: %.c Makefile
	${CC} ${CFLAGS} -g -c $<


