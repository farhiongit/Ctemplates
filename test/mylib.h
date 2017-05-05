#ifndef __MYLIB_H__

#include "set.h"
#include "map.h"

/* *INDENT-OFF* */
DECLARE_SET (int)
typedef char *pchar;
DECLARE_MAP (pchar, double)

int libfunction (SET (int) **);
/* *INDENT-ON* */

#endif
