#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mylib.h"

int
main (void)
{
  SET (int) * ps;

  libfunction (&ps);
  SET_INSERT (ps, 3);

  SNODE (int) * b = SET_BEGIN (ps);

  printf ("%i\n", *SNODE_KEY (b));

  SET_DESTROY (ps);
}
