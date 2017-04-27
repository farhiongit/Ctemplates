#include "mylib.h"
#include "set_impl.h"
#include "map_impl.h"

/* *INDENT-OFF* */
DEFINE_OPERATORS (int)
DEFINE_OPERATORS (pchar)
DEFINE_OPERATORS (double)
DEFINE_SET (int)
DEFINE_MAP (pchar, double)
/* *INDENT-ON* */

int libfunction (SET (int) ** pps)
{
  SET (int) * myset = SET_CREATE (int);

  *pps = myset;

  return myset ? EXIT_SUCCESS : EXIT_FAILURE;
}
