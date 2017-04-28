CC=clang
#CC=gcc
CFLAGS += -g -Wall -Wextra -pedantic -Werror -I..
CFLAGS += -fmacro-backtrace-limit=0
#CFLAGS += -fdebug-cpp
LDFLAGS += -pthread

all: examples

list_example.o: list_example.c ../defops.h ../list_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../list.h
set_example.o: set_example.c ../defops.h ../set_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../set.h

.PHONY: examples
examples: list_example set_example
	valgrind ./list_example || :
	valgrind ./set_example || :
