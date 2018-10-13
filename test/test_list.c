#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "defops.h"
#include "list_impl.h"

typedef char *pchar;
typedef struct
{
  int min, max;
} Range;

typedef int16_t myint;

DECLARE_LIST (myint);
DECLARE_LIST (pchar);
DECLARE_LIST (Range);

//DEFINE_DEFAULT_LESS_THAN_OPERATOR

DEFINE_OPERATORS (myint);
DEFINE_OPERATORS (pchar);
DEFINE_OPERATORS (Range);

DEFINE_LIST (myint);
DEFINE_LIST (pchar);
DEFINE_LIST (Range);

static void
sigabort_handler (int signum)
{
  fprintf (stderr, "INFO:  " "Signal %i trapped and ignored.\n" "       " "Execution continued.\n", signum);
}

static int
print_node (LNODE (myint) * n, void *param)
{
  (void) param;
  printf ("{%i}\n", *BNODE_VALUE (n));
  return EXIT_SUCCESS;
}

static void
int_destroyer (myint val)
{
  printf ("{%i} destroyed.\n", val);
}

static myint
int_copier (myint n)
{
  printf ("Copy of {%i}.\n", n);
  return n;
}

// Standard is redefined other way round
int
greater_than_int (myint a, myint b)
{
  return a > b;                 //  other way round for test purpose !
}

int
tens_less_than_int (myint a, myint b)
{
  return (((a / 10) - (b / 10)) * 10 - (a % 10 - b % 10)) < 0;
}

int
less_than_range (Range a, Range b)
{
  return (a.min * a.min + a.max * a.max < b.min * b.min + b.max * b.max);
}

static int
range_value (LNODE (myint) * n, void *param)
{
  return *BNODE_VALUE (n) < ((Range *) param)->min ? EXIT_FAILURE : *BNODE_VALUE (n) >
    ((Range *) param)->max ? EXIT_FAILURE : EXIT_SUCCESS;
}

void
phaseI_insert (LIST (myint) * l, size_t D)
{
  size_t N = 1;

  for (size_t i = 0; i < D; i++)
    N *= 2;
  N--;

  for (size_t i = 0; i < N; i++)
    LIST_APPEND (l, (myint) i); // Tree balancing is required here
}

void
phaseII_removal (LIST (myint) * l)
{
  size_t N = l->root->higher_child->size;

  for (size_t i = 0; i < N; i++)
    LIST_REMOVE (l, LIST_LAST (l));     // Tree balancing is NOT required here
}

void
phaseIII_reinsert (LIST (myint) * l, size_t N)
{
  for (size_t i = 0; i < N; i++)
    LIST_APPEND (l, (myint) LIST_SIZE (l));     // Tree balancing is required here, but only for the right branch of the tree root (because the right branch is shorter than left one, the left branch is unaffected.)
}

int
main (void)
{
  struct sigaction sa = {.sa_handler = sigabort_handler };
  sigaction (SIGABRT, &sa, 0);

  SET_DESTRUCTOR (myint, int_destroyer);
  SET_COPY_CONSTRUCTOR (myint, int_copier);

  //SET_LESS_THAN_OPERATOR (int, greater_than_int);

  LIST (myint) * la = LIST_CREATE (myint);
  printf ("-------- will abort here:\n");
  LIST_INDEX (la, 0);
  LIST_REVERSE (la);
  LIST_ROTATE_LEFT (la);
  LIST_ROTATE_RIGHT (la);
  LIST_SORT (la);

  //LIST_SET_LESS_THAN_OPERATOR (la, tens_less_than_int);

  printf ("Size %li\n", LIST_SIZE (la));

  LIST_INSERT (la, LIST_END (la), 4);
  LIST_REVERSE (la);
  LIST_ROTATE_LEFT (la);
  LIST_ROTATE_RIGHT (la);
  LIST_SORT (la);

  LNODE (myint) * na = LIST_INSERT (la, LIST_LAST (la), 3333);
  LIST_REVERSE (la);
  LIST_ROTATE_LEFT (la);
  LIST_ROTATE_RIGHT (la);
  LIST_SORT (la);

  LIST_INSERT (la, na, 2);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("Size %li\n", LIST_SIZE (la));

  printf ("%i peeked\n", *BNODE_VALUE (LIST_BEGIN (la)));

  LIST_INSERT (la, LIST_BEGIN (la), 1);

  printf ("%i peeked\n", *BNODE_VALUE (LIST_BEGIN (la)));

  LIST_INSERT (la, LIST_END (la), 5);
  printf ("Size %li\n", LIST_SIZE (la));

  //LNODE (int) *nb = LNODE_CREATE (int) ("Hello", 0, 0);   // NOK, type mismatch

  LIST_REMOVE (la, na);
  printf ("Size %li\n", LIST_SIZE (la));

  print_node (LIST_BEGIN (la), 0);
  printf ("%i peeked\n", *BNODE_VALUE (LIST_LAST (la)));
  printf ("Assign last to 99.\n");
  BNODE_ASSIGN (LIST_LAST (la), 99);
  printf ("%i peeked\n", *BNODE_VALUE (LIST_LAST (la)));

  printf ("%i %c %i\n", *BNODE_VALUE (LIST_BEGIN (la)),
          BNODE_LESS_THAN_VALUE (LIST_BEGIN (la), LIST_LAST (la)) ? '<' : '>', *BNODE_VALUE (LIST_LAST (la)));
  printf ("%i %c %i\n", *BNODE_VALUE (LIST_LAST (la)),
          BNODE_LESS_THAN_VALUE (LIST_LAST (la), LIST_BEGIN (la)) ? '<' : '>', *BNODE_VALUE (LIST_BEGIN (la)));
  printf ("%i %c %i\n", *BNODE_VALUE (BNODE_NEXT (LIST_BEGIN (la))),
          BNODE_LESS_THAN_VALUE (BNODE_NEXT (LIST_BEGIN (la)), BNODE_PREVIOUS (LIST_LAST (la))) ? '<' : '>',
          *BNODE_VALUE (BNODE_PREVIOUS (LIST_LAST (la))));

  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("Size %li\n", LIST_SIZE (la));
  printf ("-------- will abort here:\n");
  LIST_INDEX (la, LIST_SIZE (la));

  for (int i = 17; i < 28; i++)
  {
    int mulfactor = 3;

    LIST_INSERT (la, LIST_BEGIN (la), mulfactor * i);
    LIST_INSERT (la, LIST_BEGIN (la), mulfactor * i);
    LIST_INSERT (la, LIST_BEGIN (la), mulfactor * i);
    LIST_INSERT (la, LIST_END (la), mulfactor * i);
  }

  printf ("Print in reverse order.\n");
  BNODE_FOR_EACH_REVERSE (LIST_LAST (la), LIST_END (la), print_node);
  printf ("Size %li\n", LIST_SIZE (la));

  Range r = { 57, 63 };
  for (LNODE (myint) * na = LIST_BEGIN (la); na && (na = BNODE_FIND (na, LIST_END (la), range_value, &r));
       na = BNODE_NEXT (na))
    printf ("{%i} found *.\n", *BNODE_VALUE (na));

  for (LNODE (myint) * na = LIST_BEGIN (la); na && (na = LIST_FIND (la, na, 81, 0)); na = BNODE_NEXT (na))
    printf ("{%i} found **.\n", *BNODE_VALUE (na));

  for (LNODE (myint) * na = LIST_LAST (la); na && (na = BNODE_FIND_VALUE_REVERSE (na, 54)); na = BNODE_PREVIOUS (na))
    printf ("{%i} found ***.\n", *BNODE_VALUE (na));

  r.min = 75;
  r.max = 78;
  for (LNODE (myint) * na = LIST_LAST (la); na && (na = BNODE_FIND_REVERSE (na, range_value, &r));
       na = BNODE_PREVIOUS (na))
    printf ("{%i} found ****.\n", *BNODE_VALUE (na));

  printf ("%zi redundant elements removed.\n", LIST_UNIQUE (la));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("Size %li\n", LIST_SIZE (la));

  printf ("Sort.\n");
  LIST_SORT (la);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("Size %li\n", LIST_SIZE (la));

  printf ("%zi redundant elements removed again.\n", LIST_UNIQUE (la, tens_less_than_int));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("Size %li\n", LIST_SIZE (la));

  printf ("Sort again in descending order.\n");
  LIST_SORT (la, greater_than_int);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Sort again by tens.\n");
  LIST_SORT (la, tens_less_than_int);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  LNODE (myint) * aa = LIST_BEGIN (la);
  LNODE (myint) * bb = LIST_LAST (la);
  printf ("Move end at the begining....\n");
  LIST_MOVE (la, aa, la, bb);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("....\n");
  LIST_TRAVERSE (la, print_node);
  printf ("Move 2nd at the end....\n");
  LIST_MOVE (la, LIST_END (la), la, aa);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("....\n");
  LIST_TRAVERSE (la, print_node);

  printf ("Swap {%i} and {%i}.\n", *BNODE_VALUE (LIST_BEGIN (la)), *BNODE_VALUE (LIST_LAST (la)));
  LIST_SWAP (la, LIST_BEGIN (la), la, LIST_LAST (la));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);
  printf ("Size %li\n", LIST_SIZE (la));

  LIST (myint) * lb = LIST_CREATE (myint);

  LIST_INSERT (lb, LIST_BEGIN (lb), 1000);
  LIST_INSERT (lb, LIST_END (lb), 2000);
  LIST_ROTATE_LEFT (lb);
  LIST_SWAP (lb, LIST_BEGIN (lb), lb, LIST_LAST (lb));

  printf ("Move {%i} to the begining.\n", *BNODE_VALUE (LIST_LAST (la)));
  LIST_MOVE (la, LIST_BEGIN (la), la, LIST_LAST (la));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Move {%i} to the second position.\n", *BNODE_VALUE (LIST_LAST (la)));
  LIST_MOVE (la, BNODE_NEXT (LIST_BEGIN (la)), la, LIST_LAST (la));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  assert (LIST_SWAP (lb, LIST_BEGIN (lb), la, LIST_LAST (lb)) == EXIT_FAILURE);
  assert (LIST_SWAP (la, LIST_LAST (lb), lb, LIST_BEGIN (lb)) == EXIT_FAILURE);
  assert (LIST_SWAP (la, LIST_BEGIN (lb), lb, LIST_LAST (lb)) == EXIT_FAILURE);
  printf ("Swap {%i} and {%i}.\n", *BNODE_VALUE (LIST_BEGIN (lb)), *BNODE_VALUE (LIST_BEGIN (la)));
  LIST_SWAP (la, LIST_BEGIN (la), lb, LIST_BEGIN (lb));
  LIST_DESTROY (lb);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Rotate list to left.\n");
  LIST_ROTATE_LEFT (la);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Reverse [{%i}, {%i}[.\n", *BNODE_VALUE (BNODE_NEXT (LIST_BEGIN (la))), *BNODE_VALUE (LIST_LAST (la)));
  LIST_REVERSE (la, BNODE_NEXT (LIST_BEGIN (la)), LIST_LAST (la));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Reverse [{%i}, {%i}[.\n", *BNODE_VALUE (BNODE_PREVIOUS (LIST_LAST (la))),
          *BNODE_VALUE (BNODE_NEXT (LIST_BEGIN (la))));
  LIST_REVERSE (la, BNODE_PREVIOUS (LIST_LAST (la)), BNODE_NEXT (LIST_BEGIN (la)));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Reverse all.\n");
  LIST_REVERSE (la);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Rotate list to right.\n");
  LIST_ROTATE_RIGHT (la);
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Swap {%i} and {%i}.\n", *BNODE_VALUE (BNODE_NEXT (LIST_BEGIN (la))),
          *BNODE_VALUE (BNODE_PREVIOUS (LIST_LAST (la))));
  LIST_SWAP (la, BNODE_NEXT (LIST_BEGIN (la)), la, BNODE_PREVIOUS (LIST_LAST (la)));
  BNODE_FOR_EACH (LIST_BEGIN (la), LIST_END (la), print_node);

  printf ("Access through indexes.\n");
  for (size_t i = 0; i < LIST_SIZE (la); i++)
    (void) (printf ("%3zi: ", i) && print_node (LIST_INDEX (la, i), 0));

  printf ("Is empty = %i\n", LIST_IS_EMPTY (la));
  printf ("Clear.\n");
  LIST_CLEAR (la);
  printf ("Size %li\n", LIST_SIZE (la));
  printf ("Is empty = %i\n", LIST_IS_EMPTY (la));

  printf ("Destroy.\n");
  LIST_DESTROY (la);

  //LIST_TYPE(double) *da = LIST_CREATE (double);  // NOK, LIST(double) undeclared and undefined (DECLARE_LIST (double) and DEFINE_LIST (double))

  LIST (pchar) * ls = LIST_CREATE (pchar);
  LIST_INSERT (ls, LIST_END (ls), "List temaplate");
  LIST_DESTROY (ls);

  //SET_LESS_THAN_OPERATOR (Range, less_than_range);
  LIST (Range) * lp = LIST_CREATE (Range);
  Range p1 = { 1, 1 };
  Range p2 = { 2, 2 };
  Range p3 = { 3, 3 };
  LIST_INSERT (lp, LIST_BEGIN (lp), p1);
  LIST_INSERT (lp, LIST_BEGIN (lp), p3);
  LIST_INSERT (lp, LIST_BEGIN (lp), p2);

  // Using default (kind of memcmp) less than operator.
  for (LNODE (Range) * p = LIST_BEGIN (lp); p != LIST_END (lp); p = LNODE_NEXT (p))
    printf ("{ %i, %i }\n", BNODE_VALUE (p)->min, BNODE_VALUE (p)->max);
  LIST_SORT (lp);
  for (LNODE (Range) * p = LIST_BEGIN (lp); p != LIST_END (lp); p = LNODE_NEXT (p))
    printf ("{ %i, %i }\n", BNODE_VALUE (p)->min, BNODE_VALUE (p)->max);
  LIST_DESTROY (lp);

  printf ("------\n");
  SET_DESTRUCTOR (myint, 0);
  SET_COPY_CONSTRUCTOR (myint, 0);
  LIST (myint) * perf = LIST_CREATE (myint);

  size_t D = 19;

  phaseI_insert (perf, D);
  printf ("N=%1$lu, D=%2$lu [%3$lu ^ %4$lu]\n", LIST_SIZE (perf), perf->root->depth,
          perf->root->lower_child ? perf->root->lower_child->depth : 0,
          perf->root->higher_child ? perf->root->higher_child->depth : 0);

  size_t N = LIST_SIZE (perf);

  phaseII_removal (perf);
  printf ("N=%1$lu, D=%2$lu [%3$lu ^ %4$lu]\n", LIST_SIZE (perf), perf->root->depth,
          perf->root->lower_child ? perf->root->lower_child->depth : 0,
          perf->root->higher_child ? perf->root->higher_child->depth : 0);

  phaseIII_reinsert (perf, N / 16);
  printf ("N=%1$lu, D=%2$lu [%3$lu ^ %4$lu]\n", LIST_SIZE (perf), perf->root->depth,
          perf->root->lower_child ? perf->root->lower_child->depth : 0,
          perf->root->higher_child ? perf->root->higher_child->depth : 0);

  LIST_DESTROY (perf);
  printf ("------\n");
}
