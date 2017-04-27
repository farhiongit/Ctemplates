CC=clang
#CC=gcc
CFLAGS += -g -Wall -Wextra -pedantic -Werror -I..
CFLAGS += -fmacro-backtrace-limit=0
#CFLAGS += -fdebug-cpp
LDFLAGS += -pthread

all: tests main

.PHONY: test_list.expand
test_list.expand: test_list.c
	@$(CC) -E $< | indent | pygmentize -lC

.PHONY: test_set.expand
test_set.expand: test_set.c
	@$(CC) -E $< | indent | pygmentize -lC

.PHONY: test_map.expand
test_map.expand: test_map.c
	@$(CC) -E $< | indent | pygmentize -lC

.PHONY: test_tree.expand
test_tree.expand: test_tree.c
	@$(CC) -E $< | indent | pygmentize -lC

test_list.o: test_list.c ../defops.h ../list_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../list.h
test_set.o: test_set.c ../set_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../set.h ../defops.h
test_tree.o: test_tree.c ../defops.h ../bnode_impl.h ../bnode.h ../vfunc.h
test_map.o: test_map.c ../defops.h ../map_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../map.h

.PHONY: tests
tests: test_list test_set test_tree test_map
	valgrind ./test_list || :
	valgrind ./test_set || :
	valgrind ./test_tree || :
	valgrind ./test_map  || :

mylib.o: mylib.c mylib.h ../set.h ../bnode.h ../vfunc.h ../map.h ../set_impl.h ../bnode_impl.h ../defops.h ../map_impl.h
	$(CC) $(CFLAGS) -c mylib.c -o mylib.o
	strip -xg mylib.o
	nm mylib.o
	#objdump -x mylib.o

main: mylib.o main.c mylib.h ../set.h ../bnode.h ../vfunc.h ../map.h
	$(CC) $(CFLAGS) $(LDFLAGS) mylib.o main.c -o main
	strip -xg main
	nm main
	valgrind ./main
