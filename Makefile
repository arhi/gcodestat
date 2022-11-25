NOCURL := 1
STATIC := 0


CURL := /opt/homebrew/opt/curl

CC     := gcc
ARCH   := ${CURL}/lib/libcurl.a
CFLAGS := -Wall -Werror  
CFCURL := -DCURL_STATICLIB -I${CURL}/include  
LFLAGS := -L${CURL}/lib

ifeq ($(STATIC),1)
SLIBS  := -static -static-libgcc -lgcc -lws2_32 -lwldap32 
else
SLIBS  :=
endif

SLIBSx := -lpthread -static-libstdc++ -lstdc++
LIBS   := -lm 
CURLIB := -lcurl


SRCS   := gcodestat.c calcmove.c readconfig.c readgcode.c
HFILES := gcodestat.h calcmove.h readconfig.h readgcode.h

OBJS   := ${SRCS:.c=.o} 
PROGS  := gcodestat.exe

.PHONY: all
all: ${PROGS}  

${PROGS}: ${OBJS} Makefile
ifeq ($(NOCURL),1)
	${CC} -DNOCURL ${OBJS} ${SLIBS} ${LIBS} -g -o $@ 
else
	${CC}  ${OBJS} ${ARCH} ${LFLAGS} ${SLIBS} ${LIBS} ${CURLIB} -g -o $@ 
endif

clean:
	rm -f ${PROGS} ${OBJS}
	${CC} --version

%.o: %.c Makefile
ifeq ($(NOCURL),1)
	${CC} -DNOCURL ${CFLAGS} -g -c $<
else
	${CC} ${CFLAGS} ${CFCURL} -g -c $<
endif

