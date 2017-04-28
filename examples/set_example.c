#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defops.h"
#include "set_impl.h"

// Declare a user defined type compatible with templates
// 'T' is an arbitrary name.
typedef char *T;

/* *INDENT-OFF* */
// Prerequisite declarations before template usage for type T SET(T)
DECLARE_SET (T)
DEFINE_OPERATORS (T)
DEFINE_SET (T)

// Prerequisite declarations before template usage for type int SET(int)
DECLARE_SET (int)
DEFINE_OPERATORS (int)
DEFINE_SET (int)
/* *INDENT-ON* */

// Prints the content of a node
int
print_node (SNODE (T) * n, void *arg)
{
  (void) arg;
  // Access and print the content of a node
  printf ("%s;", *SNODE_KEY (n));
  return EXIT_SUCCESS;
}

// Prints the content of each node of a set
static void
set_print (SET (T) * s)
{
  // Get the number of elements in the set
  printf ("[%03lu]", SET_SIZE (s));
  // Traverse the set and apply 'print_node' to each node
  SET_TRAVERSE (s, print_node);
  printf ("\n");
}

// User defined less than operator : by length, then by alphabetic order
static int
Tlt (T a, T b)
{
  return strlen (a) < strlen (b) ? 1 : (strlen (a) == strlen (b) ? (strcoll (a, b) < 0 ? 1 : 0) : 0);
}

static int
skim (SNODE (int) * n, void *arg)
{
  int threshold = 98;

  SET (int) * si = arg;

  if (*SNODE_KEY(n) > threshold)
  {
    printf ("-(%i)-;", *SNODE_KEY(n));
    SET_REMOVE (si, n);
  }
  else
    printf ("%i;", *SNODE_KEY (n));

  return EXIT_SUCCESS;
}

int
main (void)
{
  // Create a set which type is a pointer to SET(T).
  // A predefined 'less than' operator is used here but could have been declaraed as a second argument to LIST_CREATE.
  SET (T) * myset = SET_CREATE (T, Tlt);
  set_print (myset);
  SET_FIND (myset, "aa");

  SET_INSERT (myset, "cccccccc");
  SET_INSERT (myset, "f");
  SET_INSERT (myset, "aaa");
  SET_INSERT (myset, "bbbb");
  SET_INSERT (myset, "e");
  // Uniqueness: the element bbbb is not inserted
  SET_INSERT (myset, "bbbb");
  SET_INSERT (myset, "dd");  // e;f;dd;aaa;bbbb;cccccccc;
  SET_INSERT (myset, "ZZZ");
  set_print (myset);

  // Remove all (1) elements 'ZZZ'
  for (SNODE (T) * n = 0; (n = SET_FIND (myset, "ZZZ")); SET_REMOVE (myset, n)) /* nop */ ;
  set_print (myset);

  // Get the first, second, third, second to last, and last elements of the set
  print_node (SET_BEGIN (myset), 0);
  print_node (SNODE_NEXT (SET_BEGIN (myset)), 0);
  print_node (SET_INDEX (myset, 2), 0);
  print_node (SNODE_PREVIOUS (SET_LAST (myset)), 0);
  print_node (SET_LAST (myset), 0);
  printf ("\n");

  // Find keys in the set
  char* alicia[2] = { "aa", "aaa" };
  for (size_t i = 0 ; i < sizeof (alicia) / sizeof (*alicia) ; i++)
    if (SET_FIND (myset, alicia[i]))
      printf ("%s is in the set.\n", alicia[i]);
    else
      printf ("%s is in NOT the set.\n", alicia[i]);

  SET (T) * altset = SET_CREATE (T, 0, 0);
  SET_INSERT (altset, "AAA");
  SET_INSERT (altset, "BB");
  SET_INSERT (altset, "Baobab");
  SET_INSERT (altset, "CCCC");
  // No uniqueness: the element BB is inserted a second time
  SET_INSERT (altset, "BB");
  set_print (altset);

  SET_MOVE (myset, altset, SET_FIND (altset, "Baobab"));
  SET_MOVE (myset, altset, SET_FIND (altset, "BB"));
  set_print (altset);
  SET_DESTROY (altset);
  set_print (myset);

  // Empty set
  SET_CLEAR (myset);
  if (SET_IS_EMPTY (myset))
    printf ("myset is empty.\n");;

  SET_INSERT (myset, "AAA");
  SET_INSERT (myset, "BB");
  SET_INSERT (myset, "Baobab");
  SET_INSERT (myset, "CCCC");
  // Uniqueness: the element BB is inserted a second time
  SET_INSERT (myset, "BB");
  set_print (myset);

  SET_DESTROY (myset);

  SET (int) * si = SET_CREATE (int);

  for (int i = 110; i > 0; i--)
    SET_INSERT (si, i - 1);

  // Apply skim for each element from 1 to 107 included.
  // skim removes some elements from the set.
  // Unlike SET_TRAVERSE, SET_FOR_EACH allows to remove or insert elements in the set while processing, 
  SET_FOR_EACH (si, SET_INDEX (si, 1), SNODE_PREVIOUS (SET_LAST (si)), skim, si, EXIT_FAILURE);
  printf ("\n");

  SET_DESTROY (si);
}
