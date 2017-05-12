#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defops.h"
#include "list_impl.h"

// Declare a user defined type compatible with templates
// 'T' is an arbitrary name.
typedef char *T;

/* *INDENT-OFF* */
// Prerequisite declarations before template usage for type T LIST(T)
DECLARE_LIST (T)
DEFINE_OPERATORS (T)
DEFINE_LIST (T)

// Prerequisite declarations before template usage for type int LIST(int)
DECLARE_LIST (int)
DEFINE_OPERATORS (int)
DEFINE_LIST (int)
/* *INDENT-ON* */

// Prints the content of a node
int
print_node (LNODE (T) * n, void *arg)
{
  (void) arg;
  // Access and print the content of a node
  printf ("%s;", *LNODE_VALUE (n));
  return EXIT_SUCCESS;
}

// Prints the content of each node of a list
static void
list_print (LIST (T) * l)
{
  // Get the number of elements in the list
  printf ("[%03lu]", LIST_SIZE (l));
  // Traverse the list and apply 'print_node' to each node
  LIST_TRAVERSE (l, print_node);
  printf ("\n");
}

// User defined less than operator : by decreasing length, then by inversed alphabetic order
static int
Trlt (T a, T b)
{
  return strlen (a) > strlen (b) ? 1 : (strlen (a) == strlen (b) ? (strcoll (a, b) > 0 ? 1 : 0) : 0);
}

// User defined less than operator : by length, then by alphabetic order
static int
Tlt (T a, T b)
{
  return strlen (a) < strlen (b) ? 1 : (strlen (a) == strlen (b) ? (strcoll (a, b) < 0 ? 1 : 0) : 0);
}

static int
skim (LNODE (int) * n, void *arg)
{
  int threshold = 88;

  LIST (int) * li = arg;

  if (*LNODE_VALUE (n) + 10 > threshold)
  {
    printf ("-(%i)-;", *LNODE_VALUE (n));
    LIST_REMOVE (li, n);
  }
  else
  {
    LNODE_ASSIGN (n, *LNODE_VALUE (n) + 10);
    printf ("%i;", *LNODE_VALUE (n));
  }

  return EXIT_SUCCESS;
}

int
main (void)
{
  // Create a list which type is a pointer to LIST(T).
  // A predefined 'less than' operator is used here but could have been declaraed as a second argument to LIST_CREATE.
  LIST (T) * mylist = LIST_CREATE (T);

  LIST_APPEND (mylist, "A");
  LIST_APPEND (mylist, "B");
  LIST_APPEND (mylist, "C");
  LIST_APPEND (mylist, "D");
  LIST_APPEND (mylist, "E");
  LIST_APPEND (mylist, "F");
  LIST_APPEND (mylist, "G");
  list_print (mylist);
  LIST_CLEAR (mylist);

  // Inserts elements in the list
  LIST_INSERT (mylist, LIST_END (mylist), "aaa");       // aaa
  LIST_INSERT (mylist, LIST_BEGIN (mylist), "bbbb");    // bbbb;aaa
  LIST_INSERT (mylist, LIST_END (mylist), "ccccc");     // bbbb;aaa;ccccc
  LIST_INSERT (mylist, LIST_END (mylist), "e"); // bbbb;aaa;ccccc;e
  LIST_INSERT (mylist, LIST_INDEX (mylist, LIST_SIZE (mylist) / 2), "f");       // bbbb;aaa;f;ccccc;e
  LIST_INSERT (mylist, LIST_LAST (mylist), "dd");       // bbbb;aaa;f;ccccc;dd;e
  list_print (mylist);

  // Create and populate a second list
  LIST (T) * caps = LIST_CREATE (T);
  LIST_INSERT (caps, LIST_END (caps), "YYYY");
  LIST_INSERT (caps, LIST_END (caps), "ZZZ");
  list_print (caps);

  // Move an element from one list (caps) to the other (mylist)
  LIST_MOVE (mylist, LNODE_NEXT (LIST_BEGIN (mylist)), caps, LIST_LAST (caps)); // bbbb;ZZZ;aaa;f;ccccc;dd;e
  list_print (caps);
  LIST_DESTROY (caps);

  list_print (mylist);
  // Move rhe second element before the first
  LIST_MOVE (mylist, LIST_BEGIN (mylist), mylist, LIST_INDEX (mylist, 1));      // ZZZ;bbbb;aaa;f;ccccc;dd;e
  // insert 'ZZZ' before the last element
  LIST_INSERT (mylist, LIST_LAST (mylist), "ZZZ");      // ZZZ;bbbb;aaa;f;ccccc;dd;ZZZ;e
  list_print (mylist);
  // Remove all elements 'ZZZ'
  for (LNODE (T) * n = 0; (n = LIST_FIND (mylist, "ZZZ")); LIST_REMOVE (mylist, n)) /* nop */ ;
  list_print (mylist);          // bbbb;aaa;f;ccccc;dd;e

  // Sort: standard comparator (strcoll) is applied.
  LIST_SORT (mylist);
  list_print (mylist);

  // Sort: use a user defined less than operator tied to type 'T'
  SET_LESS_THAN_OPERATOR (T, Tlt);
  LIST_SORT (mylist);
  list_print (mylist);

  // Sort: use a user defined less than operator
  LIST_SORT (mylist, Trlt);
  list_print (mylist);

  // Empty list
  LIST_CLEAR (mylist);
  if (LIST_IS_EMPTY (mylist))
    printf ("mylist is empty.\n");;

  // insert elements
  LIST_INSERT (mylist, LIST_END (mylist), "AAA");
  LIST_INSERT (mylist, LIST_END (mylist), "bBBB");
  LIST_INSERT (mylist, LIST_END (mylist), "CCCCC");
  LIST_INSERT (mylist, LIST_END (mylist), "AAA");
  LIST_INSERT (mylist, LIST_END (mylist), "dD");
  LIST_INSERT (mylist, LIST_END (mylist), "E");
  LIST_INSERT (mylist, LIST_END (mylist), "f");
  list_print (mylist);

  // Sort using a specific less than operator
  LIST_SORT (mylist, Tlt);
  list_print (mylist);

  // Remove conjoint identical elements (using an optionally defined specific less than operator)
  LIST_UNIQUE (mylist, Tlt);

  list_print (mylist);

  // Reverse list
  LIST_REVERSE (mylist);
  list_print (mylist);

  // Swap two elements (inside same list or between two different lists)
  LIST_SWAP (mylist, LIST_INDEX (mylist, 1), mylist, LIST_INDEX (mylist, LIST_SIZE (mylist) - 2));
  list_print (mylist);

  // Reverse a part of the list
  LIST_REVERSE (mylist, LIST_INDEX (mylist, 1), LIST_INDEX (mylist, LIST_SIZE (mylist) - 1));
  list_print (mylist);

  // Rotate to the left
  LIST_ROTATE_LEFT (mylist);
  list_print (mylist);

  // Rotate to the right
  LIST_ROTATE_RIGHT (mylist);
  LIST_ROTATE_RIGHT (mylist);
  list_print (mylist);

  // Set value of the last element
  char *s = strdup ("0000");

  LNODE_ASSIGN (LIST_LAST (mylist), s);
  free (s);
  list_print (mylist);

  // Set value of the first element
  LNODE_ASSIGN (LIST_BEGIN (mylist), "1111");
  list_print (mylist);

  // Access to the content of the last element
  printf ("%s\n", *LNODE_VALUE (LIST_LAST (mylist)));

  // Destroy (and deallocate resources to avoid memory leaks) after usage.
  LIST_DESTROY (mylist);

  LIST (int) * li = LIST_CREATE (int);

  for (int i = 0; i < 110; i++)
    LIST_INSERT (li, LIST_END (li), i);

  // Apply skim for each element from 1 to 107 included.
  // skim removes some elements from the list.
  // Unlike LIST_TRAVERSE, LIST_FOR_EACH allows to remove or insert elements in the list while processing, 
  LIST_FOR_EACH (li, LIST_INDEX (li, 1), LNODE_PREVIOUS (LIST_LAST (li)), skim, li, EXIT_FAILURE);
  printf ("\n");

  LIST_DESTROY (li);
}
