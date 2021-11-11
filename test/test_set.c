#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "set_impl.h"

/* *INDENT-OFF* */
//DEFINE_DEFAULT_LESS_THAN_OPERATOR

typedef int pchar;
typedef struct
{
  int min,max;
} Range;

/* *INDENT-OFF* */
DEFINE_OPERATORS (int);
DEFINE_OPERATORS (pchar);
DEFINE_OPERATORS (Range);
DEFINE_OPERATORS (double);

DECLARE_SET (int);
DECLARE_SET (pchar);
DECLARE_SET (Range);
DECLARE_SET (double);

DEFINE_SET (int);
DEFINE_SET (pchar);
DEFINE_SET (Range);
DEFINE_SET (double);
/* *INDENT-ON* */

static int
print_node (SNODE (int) * n, void *param)
{
  (void) param;
  printf ("{%i}\n", *BNODE_KEY (n));
  return 1;
}

static void
int_destroyer (int val)
{
  printf ("{%i} destroyed.\n", val);
}

static int
int_copier (int n)
{
  printf ("Copy of {%i}.\n", n);
  return n;
}

// Standard is redefined other way round
int
greater_than_int (int a, int b)
{
  return a > b;                 //  other way round for test purpose !
}

int
tens_less_than_int (int a, int b)
{
  return (a / 10) < (b / 10);
}

int
less_than_range (Range a, Range b)
{
  return (a.min * a.min + a.max * a.max < b.min * b.min + b.max * b.max);
}

static int
range_value (SNODE (int) * n, void *param)
{
  return *BNODE_KEY (n) < ((Range *) param)->min ? EXIT_FAILURE : *BNODE_KEY (n) >=
    ((Range *) param)->max ? EXIT_FAILURE : EXIT_SUCCESS;
}

void
phaseI_insert (SET (double) * l, size_t D)
{
  size_t N = 1;

  for (size_t i = 0; i < D; i++)
    N *= 2;
  N--;

  for (size_t i = 0; i < N; i++)
    SET_ADD (l, (i * 1.0) / N);      // Tree balancing is required here
}

void
phaseII_removal (SET (double) * l)
{
  while (SET_LAST (l) && SET_LAST (l) != l->root)
    SET_REMOVE (l, SET_LAST (l));       // Tree balancing is NOT required here
}

void
phaseIII_reinsert (SET (double) * l, size_t N)
{
  for (size_t i = 0; i < N; i++)
    SET_ADD (l, l->root->key + drand48 ());  // Tree balancing is sparsely required here (since the right branch is already randomly balanced, and because it is shorter than left one, the left branch is unaffected.)
}

int
main (void)
{
  SET_DESTRUCTOR (int, int_destroyer);
  SET_COPY_CONSTRUCTOR (int, int_copier);

  //SET_LESS_THAN_OPERATOR (int, greater_than_int);

  SET (int) * la = SET_CREATE (int);

  printf ("Size %li\n", SET_SIZE (la));

  SET_ADD (la, 4);
  SNODE (int) * na = SET_ADD (la, 3333);

  printf ("%i peeked\n", *BNODE_KEY (SET_FIND (la, 3333)));

  SET_ADD (la, 1);
  printf ("Size %li\n", SET_SIZE (la));

  printf ("%i peeked\n", *BNODE_KEY (SET_BEGIN (la)));

  SET_ADD (la, 2);

  printf ("%i peeked\n", *BNODE_KEY (SET_BEGIN (la)));

  SET_ADD (la, 5);
  printf ("Size %li\n", SET_SIZE (la));

  SET_REMOVE (la, na);
  printf ("Size %li\n", SET_SIZE (la));

  print_node (SET_BEGIN (la), 0);
  printf ("%i peeked\n", *BNODE_KEY (SET_LAST (la)));

  printf ("%i %c %i\n", *BNODE_KEY (SET_BEGIN (la)), BNODE_LESS_THAN_KEY (SET_BEGIN (la), SET_LAST (la)) ? '<' : '>',
          *BNODE_KEY (SET_LAST (la)));
  printf ("%i %c %i\n", *BNODE_KEY (SET_LAST (la)), BNODE_LESS_THAN_KEY (SET_LAST (la), SET_BEGIN (la)) ? '<' : '>',
          *BNODE_KEY (SET_BEGIN (la)));
  printf ("%i %c %i\n", *BNODE_KEY (BNODE_NEXT (SET_BEGIN (la))),
          BNODE_LESS_THAN_KEY (BNODE_NEXT (SET_BEGIN (la)), BNODE_PREVIOUS (SET_LAST (la))) ? '<' : '>',
          *BNODE_KEY (BNODE_PREVIOUS (SET_LAST (la))));

  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);
  printf ("Size %li\n", SET_SIZE (la));

  for (int i = 27; i > 16; i--)
  {
    SET_ADD (la, 5 * i);
    SET_ADD (la, 5 * i);
  }

  Range r = { 80, 95 };
  for (SNODE (int) * na = SET_BEGIN (la); na && (na = BNODE_FIND (na, SET_END (la), range_value, &r));
       na = BNODE_NEXT (na))
    printf ("{%i} found.\n", *BNODE_KEY (na));

  for (SNODE (int) * na = SET_BEGIN (la); na && (na = BNODE_FIND_KEY (na, 100)); na = BNODE_NEXT (na))
    printf ("{%i} found.\n", *BNODE_KEY (na));

  for (SNODE (int) * na = SET_LAST (la); na && (na = BNODE_FIND_KEY_REVERSE (na, 105)); na = BNODE_PREVIOUS (na))
    printf ("{%i} found.\n", *BNODE_KEY (na));

  r.min = 135;
  r.max = 140;
  for (SNODE (int) * na = SET_LAST (la); na && (na = BNODE_FIND_REVERSE (na, range_value, &r));
       na = BNODE_PREVIOUS (na))
    printf ("{%i} found.\n", *BNODE_KEY (na));

  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);
  printf ("Size %li\n", SET_SIZE (la));

  printf ("Sort.\n");
  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);
  printf ("Size %li\n", SET_SIZE (la));

  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);
  printf ("Size %li\n", SET_SIZE (la));

  SET (int) * lb = SET_CREATE (int, 0, 0);

  SET_ADD (lb, 1000);
  SET_ADD (lb, 2000);
  SET_ADD (lb, 1000);
  SET_ADD (lb, 2000);
  printf ("Size %li\n", SET_SIZE (lb));
  printf ("Move {%i}.\n", *BNODE_KEY (SET_BEGIN (lb)));
  SET_MOVE (la, lb, SET_BEGIN (lb));
  SET_DESTROY (lb);
  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);

  printf ("Is empty = %i\n", SET_IS_EMPTY (la));
  printf ("Clear.\n");
  SET_CLEAR (la);
  printf ("Size %li\n", SET_SIZE (la));
  printf ("Is empty = %i\n", SET_IS_EMPTY (la));

  printf ("Destroy.\n");
  SET_DESTROY (la);

  SET (pchar) * st = SET_CREATE (pchar);
  SET_DESTROY (st);

  // Using default (kind of memcmp) less than operator.
  SET (Range) * sp = SET_CREATE (Range);
  Range p1 = { 1, 1 };
  Range p2 = { 2, 2 };
  Range p3 = { 3, 3 };
  SET_ADD (sp, p1);
  SET_ADD (sp, p3);
  SET_ADD (sp, p2);
  SET_ADD (sp, p3);

  for (SNODE (Range) * p = SET_BEGIN (sp); p != SET_END (sp); p = SNODE_NEXT (p))
    printf ("{ %i, %i }\n", SNODE_KEY (p)->min, SNODE_KEY (p)->max);

  SET_DESTROY (sp);

  printf ("------\n");
  SET (double) * perf = SET_CREATE (double, 0, 0);

  size_t D = 19;

  phaseI_insert (perf, D);
  printf ("N=%1$lu, D=%2$lu [%3$lu ^ %4$lu]\n", SET_SIZE (perf), perf->root->depth,
          perf->root->lower_child ? perf->root->lower_child->depth : 0,
          perf->root->higher_child ? perf->root->higher_child->depth : 0);

  size_t N = SET_SIZE (perf);

  phaseII_removal (perf);
  printf ("N=%1$lu, D=%2$lu [%3$lu ^ %4$lu]\n", SET_SIZE (perf), perf->root->depth,
          perf->root->lower_child ? perf->root->lower_child->depth : 0,
          perf->root->higher_child ? perf->root->higher_child->depth : 0);

  phaseIII_reinsert (perf, (N - SET_SIZE (perf)) / 16);
  printf ("N=%1$lu, D=%2$lu [%3$lu ^ %4$lu]\n", SET_SIZE (perf), perf->root->depth,
          perf->root->lower_child ? perf->root->lower_child->depth : 0,
          perf->root->higher_child ? perf->root->higher_child->depth : 0);

  SET_DESTROY (perf);
  printf ("------\n");
}
