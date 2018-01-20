CURL   := E:\Dev\eclipse-workspace\curl

CC     := gcc
ARCH   := ${CURL}\lib\libcurl.a
CFLAGS := -DCURL_STATICLIB -Wall -Werror -I${CURL}\include   
LFLAGS := -L${CURL}\lib
SLIBS  := -static -static-libgcc -lgcc -lws2_32 -lwldap32 #-lpthread -static-libstdc++ -lstdc++
LIBS   := -lm -lcurl


SRCS   := gcodestat.c calcmove.c readconfig.c readgcode.c
HFILES := gcodestat.h calcmove.h readconfig.h readgcode.h

OBJS   := ${SRCS:.c=.o} 
PROGS  := gcodestat.exe

.PHONY: all
all: ${PROGS}

${PROGS}: ${OBJS} Makefile
	${CC}  ${OBJS} ${ARCH} ${LFLAGS} ${SLIBS} ${LIBS} -g -o $@ 

clean:
	rm -f ${PROGS} ${OBJS}
	${CC} --version

%.o: %.c Makefile
	${CC} ${CFLAGS} -g -c $<


