CC=clang -fmacro-backtrace-limit=0
#CC=gcc
CFLAGS += -Wall -Wextra -pedantic -Werror -I..
#CFLAGS += -pg
#CFLAGS += -fdebug-cpp
LDFLAGS += -pthread
#LDFLAGS += -pg

all: tests main gol

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
gol.o: gol.c ../set_impl.h ../bnode_impl.h ../bnode.h ../vfunc.h ../set.h ../defops.h ../map_impl.h ../map.h

gol_slow: CFLAGS += -DXXL
gol_slow: CFLAGS += -O3
gol_slow: CFLAGS += -DDEBUG=2

gol: CFLAGS += -DXXL
gol: CFLAGS += -DDEBUG=1
gol: CFLAGS += -O3
#gol: CFLAGS += -g
#gol: CFLAGS += -pg
#gol: LDFLAGS += -pg

.PHONY: run_gol
run_gol: gol
	./gol

.PHONY: tests
tests: CFLAGS += -g
tests: test_list test_set test_tree test_map
	valgrind -q ./test_list > ./test_list.tmp 2>&1 || :
	diff -q ./test_list.tmp ./test_list.log
	rm -f ./test_list.tmp
	valgrind -q ./test_set > ./test_set.tmp 2>&1 || :
	diff -q ./test_set.tmp ./test_set.log
	rm -f ./test_set.tmp
	valgrind -q ./test_tree > ./test_tree.tmp 2>&1 || :
	diff -q ./test_tree.tmp ./test_tree.log
	rm -f ./test_tree.tmp
	valgrind -q ./test_map > ./test_map.tmp 2>&1  || :
	diff -q ./test_map.tmp ./test_map.log
	rm -f ./test_map.tmp

mylib.o: mylib.c mylib.h ../set.h ../bnode.h ../vfunc.h ../map.h ../set_impl.h ../bnode_impl.h ../defops.h ../map_impl.h
	$(CC) $(CFLAGS) -c mylib.c -o mylib.o
	strip -xg mylib.o
	nm mylib.o

main.o: main.c mylib.h ../set.h ../bnode.h ../vfunc.h ../map.h
	$(CC) $(CFLAGS) -c main.c -o main.o
	strip -xg main.o
	nm main.o

main: mylib.o main.o mylib.h ../set.h ../bnode.h ../vfunc.h ../map.h
	$(CC) $(CFLAGS) $(LDFLAGS) mylib.o main.o -o main
	valgrind -q ./main
