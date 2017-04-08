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
DEFINE_OPERATORS (int)
DEFINE_OPERATORS (pchar)

DECLARE_SET (int)
DECLARE_SET (pchar)

DEFINE_SET (int)
DEFINE_SET (pchar)
/* *INDENT-ON* */

static int
print_node (SNODE (int) * n, void *param)
{
  (void)param;
  printf ("%p = %i\n", (void *) n, *BNODE_KEY (n));
  return 1;
}

static void
int_destroyer (int val)
{
  printf ("%i destroyed.\n", val);
}

static int
int_copier (int n)
{
  printf ("Copy of %i.\n", n);
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

int
main (void)
{
  SET_DESTRUCTOR (int, int_destroyer);
  SET_COPY_CONSTRUCTOR (int, int_copier);

  //SET_LESS_THAN_OPERATOR (int, greater_than_int);

  SET (int) * la = SET_CREATE (int);

  printf ("Size %li\n", SET_SIZE (la));

  SET_INSERT (la, 4);
  SNODE (int) * na = SET_INSERT (la, 3333);

  SET_INSERT (la, 1);
  printf ("Size %li\n", SET_SIZE (la));

  printf ("%i peeked\n", *BNODE_KEY (SET_BEGIN (la)));

  SET_INSERT (la, 2);

  printf ("%i peeked\n", *BNODE_KEY (SET_BEGIN (la)));

  SET_INSERT (la, 5);
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
    SET_INSERT (la, 5 * i);
    SET_INSERT (la, 5 * i);
  }

  Range r = { 80, 95 };
  for (SNODE (int) * na = SET_BEGIN (la); na && (na = BNODE_FIND (na, SET_END (la), range_value, &r));
       na = BNODE_NEXT (na))
    printf ("%p = %i found.\n", (void *) na, *BNODE_KEY (na));

  for (SNODE (int) * na = SET_BEGIN (la); na && (na = BNODE_FIND_KEY (na, 100)); na = BNODE_NEXT (na))
    printf ("%p = %i found.\n", (void *) na, *BNODE_KEY (na));

  for (SNODE (int) * na = SET_LAST (la); na && (na = BNODE_FIND_KEY_REVERSE (na, 105)); na = BNODE_PREVIOUS (na))
    printf ("%p = %i found.\n", (void *) na, *BNODE_KEY (na));

  r.min = 135;
  r.max = 140;
  for (SNODE (int) * na = SET_LAST (la); na && (na = BNODE_FIND_REVERSE (na, range_value, &r));
       na = BNODE_PREVIOUS (na))
    printf ("%p = %i found.\n", (void *) na, *BNODE_KEY (na));

  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);
  printf ("Size %li\n", SET_SIZE (la));

  printf ("Sort.\n");
  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);
  printf ("Size %li\n", SET_SIZE (la));

  BNODE_FOR_EACH (SET_BEGIN (la), SET_END (la), print_node);
  printf ("Size %li\n", SET_SIZE (la));

  SET (int) * lb = SET_CREATE (int, 0, 0);

  SET_INSERT (lb, 1000);
  SET_INSERT (lb, 2000);
  SET_INSERT (lb, 1000);
  SET_INSERT (lb, 2000);
  printf ("Size %li\n", SET_SIZE (lb));
  printf ("Move %p = %i.\n", (void *) SET_BEGIN (lb), *BNODE_KEY (SET_BEGIN (lb)));
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
}
