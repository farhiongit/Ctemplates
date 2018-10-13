/*******
 * Copyright 2017 Laurent Farhi
 *
 *  This file is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this file.  If not, see <http://www.gnu.org/licenses/>.
 *****/

////////////////////////////////////////////////////
// Original Author: Laurent Farhi
// Date:   22/1/2017
// Contact: lfarhi@sfr.fr
////////////////////////////////////////////////////

#pragma once

#ifndef __DEFOPS_H__
#define __DEFOPS_H__

static char*
__str_copy__ (char* v)
{
  if (!v)
    return 0;

  return strdup (v);
}

static void
__str_free__ (char* v)
{
  if (!v)
    return;

  free (v);
}

#define DEFINE_OPERATORS( TYPE )  \
\
  typedef TYPE (*COPY_##TYPE##_TYPE) (TYPE);              \
  typedef void (*DESTROY_##TYPE##_TYPE) (TYPE);           \
  typedef int (*LESS_THAN_##TYPE##_TYPE) (TYPE, TYPE);    \
\
  static TYPE (*COPY_##TYPE) (TYPE) = 0;    \
  static void (*DESTROY_##TYPE) (TYPE) = 0; \
  static int (*LESS_THAN_##TYPE) (TYPE, TYPE) = 0;        \
\
  static int LESS_THAN_##TYPE##_DEFAULT (TYPE a, TYPE b)  \
  {                                            \
    const size_t size = sizeof (TYPE);         \
    unsigned char *pa = (unsigned char *)(&a); \
    unsigned char *pb = (unsigned char *)(&b); \
                                               \
    for (size_t i = 0 ; i < size ; i++)        \
      if (pa[i] < pb[i])                       \
        return 1;       /* a < b */            \
      else if (pa[i] == pb[i])                 \
        continue;                              \
      else                                     \
        return 0;       /* a > b */            \
                                               \
    return 0;           /* a = b */            \
  }                                            \
  struct __useless_struct_OPS_##TYPE

/// Declares a destructor associated to type TYPE
/// @param [in] TYPE typename
/// @param [in] destructor Function with prototype void (*destructor) (TYPE)
#define SET_DESTRUCTOR( TYPE, destructor ) \
  do { DESTROY_##TYPE = destructor ; } while (0)

/// Declares a copy constructor associated to type TYPE
/// @param [in] TYPE typename
/// @param [in] constructor Function with prototype TYPE (*constructor) (TYPE)
#define SET_COPY_CONSTRUCTOR( TYPE, constructor ) \
  do { COPY_##TYPE = constructor ; } while (0)

/// Declares a less than associated to type TYPE
/// @param [in] TYPE typename
/// @param [in] operator Function with prototype int (*operator) (TYPE, TYPE)
#define SET_LESS_THAN_OPERATOR( TYPE, operator ) \
  do { LESS_THAN_##TYPE = operator ; } while (0)

#define GET_DESTRUCTOR( TYPE ) \
  (DESTROY_##TYPE)

#define GET_COPY_CONSTRUCTOR( TYPE ) \
  (COPY_##TYPE)

#define GET_LESS_THAN_OPERATOR( TYPE ) \
  (LESS_THAN_##TYPE)

#define DESTRUCTOR_TYPE( TYPE ) \
  DESTROY_##TYPE##_TYPE

#define COPY_CONSTRUCTOR_TYPE( TYPE ) \
  COPY_##TYPE##_TYPE

#define LESS_THAN_OPERATOR_TYPE( TYPE ) \
  LESS_THAN_##TYPE##_TYPE

// N.B.: A generic selection '_Generic' is NOT processed at preprocessing time but is compiled as a proper expression (at compile time).
// Even though only one assignment expression is evaluated at runtime, ...
//
//   | The controlling expression (first argument of _Generic) of a generic selection is not evaluated.
//   | If a generic selection has a generic association (type-name : assignment-expression) with a type-name that is compatible with the type of the
//   | controlling expression, then the result expression of the generic selection is the assignment-expression in that generic association.
//   | Otherwise, the result expression of the generic selection is the assignment-expression in the default generic association.
//   | None of the assignment-expressions from any other generic association of the generic selection is evaluated.
//
// ... all the assignment-expression must be valid expressions at compile time, for any possible type of the controlling expression.
// Therefore, the assignments expressions are mostly restricted to expression that do not depend on the type of the controlling expression:
// lvalues, function designators, or a void expression. Thus, in the standard, one can read: 
//
//   | The type and value of a generic selection are identical to those of its result expression.
//   | It is an lvalue, a function designator, or a void expression if its result expression is, respectively, an lvalue, a function designator, or a void expression.
//
// _Generic behaves as such as a conditional operator (cond ? expr_true : expr_false) : only one expression expr_true or expr_flase is evaluated,
// but both expressions are compiled and should be valid.

  static int __ltchar(char a, char b) { return a < b ; }
  static int __ltuchar(unsigned char a, unsigned char b) { return a < b ; }
  static int __ltshort(short a, short b) { return a < b ; }
  static int __ltushort(unsigned short a, unsigned short b) { return a < b ; }
  static int __ltint(int a, int b) { return a < b ; }
  static int __ltuint(unsigned int a, unsigned int b) { return a < b ; }
  static int __ltlong(long a, long b) { return a < b ; }
  static int __ltulong(unsigned long a, unsigned long b) { return a < b ; }
  static int __ltllong(long long a, long long b) { return a < b ; }
  static int __ltullong(unsigned long long a, unsigned long long b) { return a < b ; }
  static int __ltfloat(float a, float b) { return a < b ; }
  static int __ltdouble(double a, double b) { return a < b ; }
  static int __ltldouble(long double a, long double b) { return a < b ; }
  static int __ltstring(const char* a, const char* b) { return strcoll (a, b) < 0; }

#define LESS_THAN_DEFAULT(TYPE) _Generic(*(TYPE *)0, \
  char:               __ltchar,                   \
  unsigned char:      __ltuchar,                  \
  short:              __ltshort,                  \
  unsigned short:     __ltushort,                 \
  int:                __ltint,                    \
  unsigned int:       __ltuint,                   \
  long:               __ltlong,                   \
  unsigned long:      __ltulong,                  \
  long long:          __ltllong,                  \
  unsigned long long: __ltullong,                 \
  float:              __ltfloat,                  \
  double:             __ltdouble,                 \
  long double:        __ltldouble,                \
  char*:              __ltstring,                 \
  default:            LESS_THAN_##TYPE##_DEFAULT  \
  )

#define COPY_DEFAULT(TYPE) _Generic(*(TYPE*)0, char*:__str_copy__, default:(COPY_##TYPE##_TYPE)0)

#define DESTROY_DEFAULT(TYPE) _Generic(*(TYPE*)0, char*:__str_free__, default:(DESTROY_##TYPE##_TYPE)0)

#define PRINT_FORMAT(TYPE) _Generic(*(TYPE *)0, \
  char:               "%c",           \
  unsigned char:      "%c",           \
  short:              "%hi",          \
  unsigned short:     "%hu",          \
  int:                "%i",           \
  unsigned int:       "%u",           \
  long:               "%li",          \
  unsigned long:      "%lu",          \
  long long:          "%lli",         \
  unsigned long long: "%llu",         \
  float:              "%g",           \
  double:             "%g",           \
  long double:        "%Lg",          \
  char*:              "%s",           \
  default:            (char*)(0)      \
  )

#endif
