#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#include "defops.h"
#include "map_impl.h"

typedef char *pchar;
typedef struct
{
  double l, w, h;
} Dimensions;

/* *INDENT-OFF* */
DEFINE_OPERATORS (pchar)
DEFINE_OPERATORS (Dimensions)

DECLARE_MAP (pchar, Dimensions)
DEFINE_MAP (pchar, Dimensions)
/* *INDENT-ON* */

static int
print_car (BNODE (pchar, Dimensions) * car, void *arg)
{
  (void) arg;
  printf ("%s (%g, %g, %g)\n", *BNODE_KEY (car), BNODE_VALUE (car)->l, BNODE_VALUE (car)->w, BNODE_VALUE (car)->h);

  return EXIT_SUCCESS;
}

int
Dimensions_lt (Dimensions a, Dimensions b)
{
  return a.l < b.l || (a.l == b.l && a.w < b.w) || (a.l == b.l && a.w == b.w && a.h < b.h);
}

int
main (void)
{
  setlocale (LC_ALL, "");

  MAP (pchar, Dimensions) * cars = MAP_CREATE (pchar, Dimensions);

  Dimensions rt = {.l = 3595,.w = 1647,.h = 1557 };
  Dimensions cc1 = {.l = 3466,.w = 1615,.h = 1460 };
  Dimensions p108 = {.l = 3475,.w = 1615,.h = 1460 };
  MAP_INSERT (cars, "Renault Twingo", cc1);     // Inserts and sets value
  MAP_SET_VALUE (cars, "Renault Twingo", rt);   // Does not insert but sets value
  MAP_SET_VALUE (cars, "Citroën C1", cc1);     // Inserts and sets value
  MAP_INSERT (cars, "Citroën C1", rt); // Does neither insert nor modify value
  MAP_SET_VALUE (cars, "Peugeot 108", cc1);     // Inserts and sets value
  MAP_SET_VALUE (cars, "Peugeot 108", p108);    // Does not insert but sets value

  MAP (pchar, Dimensions) * fiat = MAP_CREATE (pchar, Dimensions);

  Dimensions mini3 = {.l = 3821,.w = 1727,.h = 1415 };
  MAP_SET_VALUE (fiat, "Mini Cooper", mini3);
  Dimensions f500 = {.l = 3546,.w = 1627,.h = 1488 };
  MAP_SET_VALUE (fiat, "Fiat 500", f500);

  MAP_MOVE (cars, fiat, MAP_KEY (fiat, "Fiat 500"));
  MAP_REMOVE (fiat, MAP_KEY (fiat, "Mini Cooper"));
  printf ("%lu elements in fiat\n", MAP_SIZE (fiat));

  MAP_DESTROY (fiat);

  MAP_TRAVERSE (cars, print_car);

  MAP (pchar, Dimensions) * minis = MAP_CREATE (pchar, Dimensions, 0, 0);       // no unicity
  MAP_SET_VALUE (minis, "Mini Cooper 3", mini3);
  MAP_INSERT (minis, "Mini Cooper 5", mini3);
  Dimensions mini5 = {.l = 4005,.w = 1727,.h = 1428 };
  MAP_SET_VALUE (minis, "Mini Cooper 5", mini5);        // Reset value
  MAP_INSERT (minis, "Mini Cooper 5", mini5);   // Duplicated
  printf ("%lu elements in minis\n", MAP_SIZE (minis));
  MAP_TRAVERSE (minis, print_car);
  MAP_DESTROY (minis);

  // Find keys in the set
  char *alicia[2] = { "Fiat 500", "Mini Cooper" };
  for (size_t i = 0; i < sizeof (alicia) / sizeof (*alicia); i++)
    if (MAP_FIND_KEY (cars, alicia[i]))
      printf ("%s is in the map.\n", alicia[i]);
    else
      printf ("%s is in NOT the map.\n", alicia[i]);

  Dimensions d = {.l = 3546,.w = 1627,.h = 1488 };

  MAP_SET_LESS_THAN_VALUE_OPERATOR (cars, Dimensions_lt);
  BNODE (pchar, Dimensions) * c = MAP_FIND_VALUE (cars, d);
  if (c)
    printf ("%s\n", *BNODE_KEY (c));

  c = MAP_INDEX (cars, 1);
  if (c)
    printf ("%s\n", *BNODE_KEY (c));

  c = MAP_LAST (cars);
  if (c)
    printf ("%s\n", *BNODE_KEY (c));

  MAP_DESTROY (cars);

  //TODO: - define equality operator for lists, sets, maps (keys and values)
}
