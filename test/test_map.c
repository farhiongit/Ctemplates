#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <assert.h>
#include <signal.h>
#include <pthread.h>

#define PTHREAD_SET_SPECIFIC(key, value, TYPE, TYPE_destructor) \
do {                                     \
  TYPE *old = pthread_getspecific (key); \
  TYPE *new = malloc (sizeof (*new));    \
  *new = value;                          \
  if (TYPE_destructor && old)            \
    TYPE_destructor (old);               \
  pthread_setspecific (key, new);        \
} while (0)

static void
sigaction_param_destructor (void *sigaction_param_value)
{
  fprintf (stderr, "%s (%p)\n", __func__, sigaction_param_value);
  fflush (stderr);
  free (sigaction_param_value);
}

static pthread_once_t pthread_local_init = PTHREAD_ONCE_INIT;
static pthread_key_t sigaction_param;
static void (*sigaction_param_destructor_function) (void *sigaction_param_value) = sigaction_param_destructor;

static void
create_pthread_local_vars (void)
{
  pthread_key_create (&sigaction_param, sigaction_param_destructor_function);
}

#include "defops.h"
#include "map_impl.h"

typedef char *pchar;
typedef struct
{
  double x, y;
} Point;

/* *INDENT-OFF* */
DECLARE_MAP (int, int)
DECLARE_MAP (pchar, double)
DECLARE_MAP (Point, int)

DEFINE_OPERATORS (pchar)
DEFINE_OPERATORS (int)
DEFINE_OPERATORS (double)
DEFINE_OPERATORS (Point)

DEFINE_MAP (int, int)
DEFINE_MAP (pchar, double)
DEFINE_MAP (Point, int)
/* *INDENT-ON* */

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

static void
double_destroyer (double val)
{
  printf ("%#g destroyed.\n", val);
}

static double
double_copier (double val)
{
  printf ("Copy of %#g.\n", val);
  return val;
}

static void
string_destroyer (char *val)
{
  printf ("%s destroyed.\n", val);
}

static char *
string_copier (char *val)
{
  printf ("Copy of %s.\n", val);
  return val;
}

static int
print_node (BNODE (pchar, double) * node, void *param)
{
  (void) param;
  printf ("%s=%g ; ", *BNODE_KEY (node), *BNODE_VALUE (node));
  return EXIT_SUCCESS;
}

static int
twice_trim (BNODE (pchar, double) * node, void *param)
{
  MAP (pchar, double) * m = param;

  BNODE_ASSIGN (node, 2 * *BNODE_VALUE (node));
  if (*BNODE_VALUE (node) > 100)
    assert (MAP_REMOVE (m, node) == EXIT_SUCCESS);

  return EXIT_SUCCESS;
}

static int
pointless (Point a, Point b)
{
  return a.x < b.x;
}

_Thread_local
MAP (pchar, double) *
  local_map;

     int main (void)
{
  setlocale (LC_ALL, "");

  pthread_once (&pthread_local_init, create_pthread_local_vars);
  PTHREAD_SET_SPECIFIC (sigaction_param, 1, int, sigaction_param_destructor_function);

  SET_DESTRUCTOR (int, int_destroyer);
  SET_COPY_CONSTRUCTOR (int, int_copier);
  SET_DESTRUCTOR (double, double_destroyer);
  SET_COPY_CONSTRUCTOR (double, double_copier);

  SET_DESTRUCTOR (pchar, string_destroyer);
  SET_COPY_CONSTRUCTOR (pchar, string_copier);

  BNODE (pchar, double) * na = BNODE_CREATE (pchar, double) ("-3-", 0);
  BNODE (pchar, double) * nb = BNODE_CREATE (pchar, double) ("-2-", 0);
  BNODE (pchar, double) * nc = BNODE_COPY (nb);

  BNODE_ASSIGN (na, 3.58);
  BNODE_ASSIGN (nb, 4.58);
  BNODE_ASSIGN (nc, 5.58);

  printf ("%i\n", BNODE_LESS_THAN_KEY (na, nb));
  printf ("%i\n", BNODE_LESS_THAN_KEY (nb, na));
  printf ("%i\n", BNODE_LESS_THAN_KEY (nb, nc));
  printf ("%i\n", BNODE_LESS_THAN_VALUE (na, nb));
  printf ("%i\n", BNODE_LESS_THAN_VALUE (nb, na));
  printf ("%i\n", BNODE_LESS_THAN_VALUE (nb, nc));

  printf ("%i\n", BNODE_CMP_KEY (na, nb));
  printf ("%i\n", BNODE_CMP_KEY (nb, na));
  printf ("%i\n", BNODE_CMP_KEY (nb, nc));
  printf ("%i\n", BNODE_CMP_VALUE (na, nb));
  printf ("%i\n", BNODE_CMP_VALUE (nb, na));
  printf ("%i\n", BNODE_CMP_VALUE (nb, nc));

  BNODE_DESTROY (na);
  BNODE_DESTROY (nb);
  BNODE_DESTROY (nc);

  MAP (Point, int) * mp = MAP_CREATE (Point, int, pointless);
  Point p1 = { 1, 1 };
  Point p2 = { 1, 1 };
  MAP_SET_VALUE (mp, p1, 1);
  MAP_SET_VALUE (mp, p2, 2);
  MAP_DESTROY (mp);

  MAP (int, int) * mb = MAP_CREATE (int, int);

  MAP_INSERT (mb, 3, 0);
  BNODE (int, int) * two = MAP_INSERT (mb, 2, 0);

  MAP_INSERT (mb, 4, 0);
  MAP_REMOVE (mb, two);
  (void) MAP_SIZE (mb);
  (void) MAP_IS_EMPTY (mb);
  MAP_DESTROY (mb);

  MAP (pchar, double) * ma = MAP_CREATE (pchar, double, 0, 1);

  MAP_INSERT (ma, "Julietta", 37.2);
  na = MAP_INSERT (ma, "Alberto", 45.2);
  MAP_INSERT (ma, "Raimondo", 77.2);
  BNODE_ASSIGN (BNODE_FIND_KEY (MAP_BEGIN (ma), "Julietta", ma->LessThan), 27.2);
  MAP_INSERT (ma, "Bernardo", 0);

  MAP (pchar, double) * msf = MAP_CREATE (pchar, double);

  na = MAP_INSERT (msf, "X", 0);
  MAP_INSERT (msf, "Y", 0);
  nb = MAP_INSERT (msf, "Z", 0);
  MAP_INSERT (msf, "T", 0);

  MAP_TRAVERSE (msf, print_node);
  printf ("*\n");

  MAP_MOVE (ma, msf, na);
  MAP_MOVE (ma, msf, nb);

  MAP_TRAVERSE (msf, print_node);
  printf ("*\n");

  MAP_DESTROY (msf);

  MAP_TRAVERSE (ma, print_node);
  printf ("\n");

  na = MAP_KEY (ma, "Raimondo");
  print_node (na, 0);
  printf ("\n");

  na = MAP_FIND_VALUE (ma, 27.2);
  print_node (na, 0);
  printf ("\n");

  na = MAP_SET_VALUE (ma, "Zorro", 55.5);
  print_node (na, 0);
  printf ("\n");

  na = MAP_SET_VALUE (ma, "Ingrid", 33.3);
  na = MAP_SET_VALUE (ma, "Ingrid", 22.2);
  print_node (na, 0);
  printf ("\n");

  assert (MAP_FIND_KEY (ma, "Raimonda") == 0);
  MAP_INSERT (ma, "Raimonda", 0);
  assert (MAP_FIND_KEY (ma, "Raimondo") == MAP_INSERT (ma, "Raimondo", 0));
  MAP_SET_VALUE (ma, "Alberta", 44.4);
  MAP_INSERT (ma, "Zu", 0);

  printf ("Size=%zi\n", MAP_SIZE (ma));
  MAP_TRAVERSE (ma, print_node);
  printf ("\n");
  MAP_FOR_EACH (ma, twice_trim, ma);
  MAP_TRAVERSE (ma, print_node);
  printf ("\n");

  for (BNODE (pchar, double) * n = MAP_BEGIN (ma); n && (n = MAP_FIND_KEY (ma, n, "Alberto")); n = BNODE_NEXT (n))
    print_node (n, 0);
  printf ("\n");

  for (BNODE (pchar, double) * n = MAP_BEGIN (ma); n && (n = MAP_FIND_VALUE (ma, n, 2 * 22.2)); n = BNODE_NEXT (n))
    print_node (n, 0);
  printf ("\n");

  MAP_CLEAR (ma);

  for (BNODE (pchar, double) * n = MAP_BEGIN (ma); n && (n = MAP_FIND_KEY (ma, n, "Alberto")); n = BNODE_NEXT (n))
    print_node (n, 0);
  printf ("*\n");

  for (BNODE (pchar, double) * n = MAP_BEGIN (ma); n && (n = MAP_FIND_VALUE (ma, n, 2 * 22.2)); n = BNODE_NEXT (n))
    print_node (n, 0);
  printf ("*\n");

  printf ("Size=%zi\n", MAP_SIZE (ma));
  MAP_TRAVERSE (ma, print_node);
  printf ("*\n");

  MAP_SET_VALUE (ma, "C", 100);
  MAP_SET_VALUE (ma, "B", 2);
  MAP_SET_VALUE (ma, "A", 1);
  MAP_SET_VALUE (ma, "D", 8);
  MAP_FOR_EACH (ma, print_node);
  printf ("\n");
  assert (MAP_FIND_KEY (ma, "A") != 0);
  assert (MAP_FIND_KEY (ma, "B") != 0);
  assert (MAP_FIND_KEY (ma, "C") != 0);
  assert (MAP_FIND_KEY (ma, "D") != 0);
  MAP_FOR_EACH (ma, twice_trim);        // MAP_FOR_EACH allows removing elements during traversing
  MAP_FOR_EACH (ma, print_node);
  printf ("\n");
  assert (MAP_FIND_KEY (ma, "A") != 0);
  assert (MAP_FIND_KEY (ma, "B") != 0);
  assert (MAP_FIND_KEY (ma, "C") == 0);
  assert (MAP_FIND_KEY (ma, "D") != 0);
  assert (*BNODE_VALUE (MAP_KEY (ma, "A")) == 2);
  assert (*BNODE_VALUE (MAP_KEY (ma, "B")) == 4);
  assert (*BNODE_VALUE (MAP_KEY (ma, "D")) == 16);
  MAP_FOR_EACH (ma, print_node);
  printf ("\n");
  MAP_CLEAR (ma);

  MAP_SET_VALUE (ma, "C", 100);
  MAP_SET_VALUE (ma, "B", 1);
  MAP_SET_VALUE (ma, "A", 1);
  MAP_SET_VALUE (ma, "D", 1);
  MAP_TRAVERSE (ma, print_node);
  printf ("\n");
  //MAP_TRAVERSE (ma, twice_trim);  // MAP_TRAVERSE does not allow removing elements during traversing
  MAP_INDEX (ma, 0);
  //MAP_INDEX (ma, 10);
  MAP_CLEAR (ma);

  MAP (int, int) * mi = MAP_CREATE (int, int);

  MAP_SET_VALUE (mi, 25, 25);
  for (int i = 0; i < 50; i++)
    MAP_SET_VALUE (mi, rand () % 100, -rand () % 100);
  MAP_SET_VALUE (mi, 75, 75);
  for (int i = 0; i < 50; i++)
    MAP_SET_VALUE (mi, rand () % 100, -rand () % 100);
  MAP_SET_VALUE (mi, 75, 750);
  MAP_SET_VALUE (mi, 25, 250);
  MAP_REMOVE (mi, MAP_FIND_KEY (mi, 25));
  MAP_REMOVE (mi, MAP_FIND_KEY (mi, 75));
  for (BNODE (int, int) * n = MAP_BEGIN (mi); n; n = BNODE_NEXT (n))
    printf ("%i=%i ; ", *BNODE_KEY (n), *BNODE_VALUE (n));
  printf ("\n");

  MAP_DESTROY (mi);

  MAP_DESTROY (ma);

  pthread_exit (0);
}
