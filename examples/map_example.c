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
  MAP_SET_VALUE (cars, "Renault Twingo", rt);
  MAP_SET_VALUE (cars, "Citroën C1", cc1);
  MAP_SET_VALUE (cars, "Peugeot 108", p108);

  MAP (pchar, Dimensions) * fiat = MAP_CREATE (pchar, Dimensions);

  Dimensions f500 = {.l = 3546,.w = 1627,.h = 1488 };
  MAP_SET_VALUE (fiat, "Fiat 500", f500);
  Dimensions mini = {.l = 3821,.w = 1727,.h = 1415 };
  MAP_SET_VALUE (fiat, "Mini Cooper", mini);

  MAP_MOVE (cars, fiat, MAP_KEY (fiat, "Fiat 500"));
  MAP_REMOVE (fiat, MAP_KEY (fiat, "Mini Cooper"));
  printf ("%lu elements in fiat\n", MAP_SIZE (fiat));

  MAP_DESTROY (fiat);

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

  //TODO: - remove %AP_INSERT
  //      - define equality operator for lists, sets, maps (keys and values)
}
