#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "defops.h"
#include "bnode_impl.h"

typedef char *pchar;

/* *INDENT-OFF* */
DECLARE_BNODE (pchar, int);

DEFINE_OPERATORS (pchar);
DEFINE_OPERATORS (int);

DEFINE_BNODE (pchar, int);
/* *INDENT-ON* */

static void
sigabort_handler (int signum)
{
  fprintf (stderr, "INFO:  " "Signal %i trapped and ignored.\n" "       " "Execution continued.\n", signum);
}

static int
print_node_key (BNODE (pchar, int) * node, void *param)
{
  (void) param;
  printf ("%s ; ", *BNODE_KEY (node));
  return EXIT_SUCCESS;
}

static int
print_node_full (BNODE (pchar, int) * n, void *param)
{
  (void) param;
  printf ("%s %i,\t\t>%s <%s ^%s,\t\t>>%s <<%s\t\tsize=%zi, depth=%zi\n", *BNODE_KEY (n), *BNODE_VALUE (n),
          n->lower_child ? *BNODE_KEY (n->lower_child) : "(null)",
          n->higher_child ? *BNODE_KEY (n->higher_child) : "(null)",
          n->parent ? *BNODE_KEY (n->parent) : "(null)",
          n->lowest_child ? *BNODE_KEY (n->lowest_child) : "(null)",
          n->highest_child ? *BNODE_KEY (n->highest_child) : "(null)", n->size, n->depth);
  return EXIT_SUCCESS;
}

int
main (void)
{
  struct sigaction sa = {.sa_handler = sigabort_handler };
  sigaction (SIGABRT, &sa, 0);

  int unique = 0;

  BNODE (pchar, int) * tree = BNODE_CREATE (pchar, int) ("Laurent", unique);
  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Ingrid", unique), 0);
  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Emmanuel", unique), 0);
  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Alphonse", unique), 0);
  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Rosella", unique), 0);
  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Emmanuel", unique), 0);
  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Zorro", unique), 0);
  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Emmanuel", unique), 0);

  BNODE (pchar, int) * tree2 = BNODE_CREATE (pchar, int) ("C", unique);
  BNODE_TREE_ADD (tree2, BNODE_CREATE (pchar, int) ("A", unique), 0);
  BNODE_TREE_ADD (tree2, BNODE_CREATE (pchar, int) ("F", unique), 0);

  BNODE_TREE_ADD (tree, tree2, 0);
  BNODE_TRAVERSE (tree, print_node_full, 0, EXIT_FAILURE);

  BNODE_REMOVE (tree2);
  BNODE_DESTROY (tree2);

  printf ("--------\n");

  tree2 = BNODE_CREATE (pchar, int) ("X", unique);
  BNODE_TREE_ADD (tree2, BNODE_CREATE (pchar, int) ("Y", unique), 0);
  BNODE_TREE_ADD (tree2, BNODE_CREATE (pchar, int) ("Z", unique), 0);

  BNODE (pchar, int) * newRoot = BNODE_REMOVE (tree2);

  BNODE_TRAVERSE (newRoot, print_node_full, 0, EXIT_FAILURE);
  BNODE_DESTROY (newRoot);

  BNODE_TREE_ADD (tree, tree2, 0);
  printf ("%zi [%s, %s]\n", tree->size, *BNODE_KEY (tree->lowest_child), *BNODE_KEY (tree->highest_child));
  BNODE_TRAVERSE (tree, print_node_full, 0, EXIT_FAILURE);

  printf ("--------\n");

  for (BNODE (pchar, int) * n = BNODE_FIRST (tree); n; n = BNODE_NEXT (n))
    print_node_full (n, 0);

  printf ("--------\n");

  printf ("%zi [%s, %s]\n", tree->size, *BNODE_KEY (tree->lowest_child), *BNODE_KEY (tree->highest_child));
  for (BNODE (pchar, int) * n = BNODE_LAST (tree); n; n = BNODE_PREVIOUS (n))
    print_node_full (n, 0);

  printf ("--------\n");

  newRoot = BNODE_REMOVE (tree);
  BNODE_TRAVERSE (newRoot, print_node_full, 0, EXIT_FAILURE);
  printf ("--------\n");
  BNODE_TREE_ADD (newRoot, tree, 0);
  tree = newRoot;
  BNODE_TRAVERSE (tree, print_node_full, 0, EXIT_FAILURE);
  printf ("%zi [%s, %s]\n", tree->size, *BNODE_KEY (tree->lowest_child), *BNODE_KEY (tree->highest_child));

  printf ("--------\n");

  for (size_t i = 0; i < tree->size; i++)
  {
    BNODE (pchar, int) * n = BNODE_INDEX (tree, i);

    printf ("%zi: ", i);
    print_node_full (n, 0);
  }

  printf ("--------\n");

  BNODE_TRAVERSE (tree, print_node_key, 0, EXIT_FAILURE);
  printf ("\n");

  BNODE_FOR_EACH (BNODE_FIRST (tree), 0, print_node_key);
  printf ("\n");

  BNODE_FOR_EACH_REVERSE (BNODE_LAST (tree), 0, print_node_key);
  printf ("\n");

  printf ("--------\n");

  BNODE_TREE_ADD (tree, BNODE_CREATE (pchar, int) ("Emmanuel", unique), 0);
  for (BNODE (pchar, int) * n = BNODE_FIRST (tree); n && (n = BNODE_FIND_KEY (n, "Emmanuel", 0)); n = BNODE_NEXT (n))
    print_node_full (n, 0);

  printf ("--------\n");

  for (BNODE (pchar, int) * n = BNODE_FIRST (tree); n; n = BNODE_NEXT (n))
  {
    char alien[100];

    snprintf (alien, sizeof (alien) / sizeof (*alien), "_alien_[%s]", *BNODE_KEY (n));
    BNODE_TREE_INSERT_BEFORE (tree, n, BNODE_CREATE (pchar, int) (alien, unique));
  }
  BNODE_TREE_INSERT_BEFORE (tree, 0, BNODE_CREATE (pchar, int) ("_alien_end", unique));
  BNODE_TREE_INSERT_BEFORE (tree, BNODE_FIRST (tree), BNODE_CREATE (pchar, int) ("_alien_begining", unique));

  BNODE_TRAVERSE (tree, print_node_full, 0, EXIT_FAILURE);

  printf ("-------- will abort here:\n");

  (void) BNODE_INDEX (tree, tree->size);

  BNODE_DESTROY (tree);
}
