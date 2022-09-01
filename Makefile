LUA_INC=-I /usr/local/include
LUA_LIB=-L /usr/local/bin -llua54

CFLAGS=-O2 -Wall
SHARED=--shared -fPIC

all : roadmap.dll

roadmap.dll : roadmap.c
	gcc $(CFLAGS) $(SHARED) -o $@ $^ $(LUA_INC) $(LUA_LIB)

clean :
	rm -f roadmap.dll


