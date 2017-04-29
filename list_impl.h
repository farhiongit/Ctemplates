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
//
// Based on design from Randy Gaul's Game Programming Blog http://cecilsunkure.blogspot.fr/2012/08/generic-programming-in-c.html
////////////////////////////////////////////////////

#pragma once

#ifndef __LIST_IMPL_H__
#define __LIST_IMPL_H__

#define _GNU_SOURCE
#include <stdlib.h>
#include <errno.h>
#include "bnode_impl.h"
#include "list.h"
#include "defops.h"

DEFINE_OPERATORS(__list_dummy__)

//
// DEFINE_LIST
// Purpose: Defines a linked list of a specific type on the heap.
//
#define DEFINE_LIST( TYPE )                              \
\
  DEFINE_BNODE(__list_dummy__, TYPE)                     \
\
  static void LIST_CLEAR_##TYPE ( LIST_##TYPE *self );                                                               \
  static void LIST_DESTROY_##TYPE ( LIST_##TYPE *self );                                                             \
  static BNODE___list_dummy___##TYPE *LIST_INSERT_##TYPE ( LIST_##TYPE *self, BNODE___list_dummy___##TYPE *here, BNODE___list_dummy___##TYPE *node );             \
  static int LIST_REMOVE_##TYPE ( LIST_##TYPE *self, BNODE___list_dummy___##TYPE *node );                            \
  static size_t LIST_UNIQUE_##TYPE ( LIST_##TYPE *self, int (*less_than) (TYPE, TYPE));                              \
  static void LIST_SORT_##TYPE ( LIST_##TYPE *self, int (*less_than) (TYPE, TYPE));                                  \
  static int LIST_MOVE_##TYPE ( LIST_##TYPE *to, BNODE___list_dummy___##TYPE *hereto, LIST_##TYPE *from, BNODE___list_dummy___##TYPE *herefrom ); \
  static int LIST_SWAP_##TYPE ( LIST_##TYPE *la, BNODE___list_dummy___##TYPE *nodea, LIST_##TYPE *lb, BNODE___list_dummy___##TYPE *nodeb ); \
  static int LIST_REVERSE_##TYPE ( LIST_##TYPE *self, BNODE___list_dummy___##TYPE *nodea, BNODE___list_dummy___##TYPE *nodeb ); \
  static BNODE___list_dummy___##TYPE *LIST_END_##TYPE ( LIST_##TYPE *self );                                         \
\
  static BNODE___list_dummy___##TYPE LIST_NULL_##TYPE = { .vtable = &BNODE_VTABLE___list_dummy___##TYPE };           \
\
  static const _LIST_VTABLE_##TYPE LIST_VTABLE_##TYPE =  \
  {                                                      \
    BNODE_CREATE___list_dummy___##TYPE,                  \
    LIST_CLEAR_##TYPE,                                   \
    LIST_DESTROY_##TYPE,                                 \
    LIST_INSERT_##TYPE,                                  \
    LIST_REMOVE_##TYPE,                                  \
    LIST_UNIQUE_##TYPE,                                  \
    LIST_SORT_##TYPE,                                    \
    LIST_MOVE_##TYPE,                                    \
    LIST_SWAP_##TYPE,                                    \
    LIST_REVERSE_##TYPE,                                 \
    LIST_END_##TYPE,                                     \
  };                                                     \
\
  LIST_##TYPE *LIST_CREATE_##TYPE( int (*less_than_operator) (TYPE, TYPE) )   \
  {                                                                           \
    LIST_##TYPE *linkedList = malloc( sizeof( *linkedList ) );                \
    if (!linkedList)                                                          \
      return 0;                                                               \
                                                                              \
    linkedList->vtable = &LIST_VTABLE_##TYPE;                                 \
    linkedList->LessThan = less_than_operator;                                \
    linkedList->tree_locked = 0;                                              \
    linkedList->root = 0;                                                     \
    linkedList->null = &LIST_NULL_##TYPE;                                     \
                                                                              \
    return linkedList;                                                        \
  }                                                                           \
\
  static BNODE___list_dummy___##TYPE *LIST_INSERT_##TYPE ( LIST_##TYPE *self, BNODE___list_dummy___##TYPE *here, BNODE___list_dummy___##TYPE *node )  \
  {                                                                        \
    if (node->parent)                                                      \
    {                                                                      \
      errno = EINVAL;                                                      \
      return 0;                                                            \
    }                                                                      \
                                                                           \
    if (here)                                                              \
    {                                                                      \
      /* Go to root */                                                     \
      BNODE___list_dummy___##TYPE *n;                                      \
      for (n = here ; n->parent ; n = n->parent) /* nop */ ;               \
      /* Check that herefrom is owned by from */                           \
      if (n != self->root)                                                 \
      {                                                                    \
        errno = EINVAL;                                                    \
        return 0;                                                          \
      }                                                                    \
    }                                                                      \
                                                                           \
    if (self->tree_locked)                                                 \
    {                                                                      \
      fprintf (stderr, "ERROR: " "Collection cannot be modified here.\n"   \
                       "ABORT  " "\n");                                    \
      fflush (0) ; raise (SIGABRT);                                        \
      return 0;                                                            \
    }                                                                      \
                                                                           \
    if (self->root)                                                        \
      BNODE_TREE_INSERT_BEFORE (self->root, here, node);                   \
    else                                                                   \
      self->root = node;                                                   \
                                                                           \
    return node;                                                           \
  }                                                                        \
\
  static int LIST_REMOVE_##TYPE ( LIST_##TYPE *self, BNODE___list_dummy___##TYPE *node )   \
  {                                                                        \
    if (!self->root)                                                       \
    {                                                                      \
      errno = EINVAL;                                                      \
      return EXIT_FAILURE;                                                 \
    }                                                                      \
                                                                           \
    /* Go to root */                                                       \
    BNODE___list_dummy___##TYPE *n;                                        \
    for (n = node ; n->parent ; n = n->parent) /* nop */ ;                 \
    /* Check that herefrom is owned by from */                             \
    if (n != self->root)                                                   \
    {                                                                      \
      errno = EINVAL;                                                      \
      return EXIT_FAILURE;                                                 \
    }                                                                      \
                                                                           \
    if (self->tree_locked)                                                 \
    {                                                                      \
      fprintf (stderr, "ERROR: " "Collection cannot be modified here.\n"   \
                       "ABORT  " "\n");                                    \
      fflush (0) ; raise (SIGABRT);                                        \
      return EXIT_FAILURE;                                                 \
    }                                                                      \
                                                                           \
    BNODE___list_dummy___##TYPE *new_node = BNODE_REMOVE___list_dummy___##TYPE(node);       \
    if (self->root == node)                                                \
      self->root = new_node;                                               \
    BNODE_DESTROY___list_dummy___##TYPE(node);                             \
                                                                           \
    return EXIT_SUCCESS;                                                   \
  }                                                                        \
\
  static int LIST_MOVE_##TYPE ( LIST_##TYPE *to, BNODE___list_dummy___##TYPE *hereto, LIST_##TYPE *from, BNODE___list_dummy___##TYPE *herefrom )                         \
  {                                                                        \
    if (!from->root)                                                       \
    {                                                                      \
      errno = EINVAL;                                                      \
      return EXIT_FAILURE;                                                 \
    }                                                                      \
                                                                           \
    /* Go to root */                                                       \
    BNODE___list_dummy___##TYPE *n;                                        \
    for (n = herefrom ; n->parent ; n = n->parent) /* nop */ ;             \
    /* Check that herefrom is owned by from */                             \
    if (n != from->root)                                                   \
    {                                                                      \
      errno = EINVAL;                                                      \
      return EXIT_FAILURE;                                                 \
    }                                                                      \
                                                                           \
    if (from->tree_locked)                                                 \
    {                                                                      \
      fprintf (stderr, "ERROR: " "Collection cannot be modified here.\n"   \
                       "ABORT  " "\n");                                    \
      fflush (0) ; raise (SIGABRT);                                        \
      return EXIT_FAILURE;                                                 \
    }                                                                      \
                                                                           \
    BNODE___list_dummy___##TYPE *new_node = BNODE_REMOVE___list_dummy___##TYPE(herefrom);       \
    if (from->root == herefrom)                                            \
      from->root = new_node;                                               \
    if (!LIST_INSERT_##TYPE (to, hereto, herefrom))                        \
    {                                                                      \
      errno = EINVAL;                                                      \
      return EXIT_FAILURE;                                                 \
    }                                                                      \
                                                                           \
    return EXIT_SUCCESS;                                                   \
  }                                                                        \
\
  static void LIST_CLEAR_##TYPE ( LIST_##TYPE *self )  \
  {                                                    \
    if (self->root)                                    \
      BNODE_DESTROY___list_dummy___##TYPE(self->root); \
    self->root = 0;                                    \
  }                                                    \
\
  static void LIST_DESTROY_##TYPE ( LIST_##TYPE *self )  \
  {                                    \
    LIST_CLEAR_##TYPE (self);          \
    free( self );                      \
  }                                    \
\
  static size_t LIST_UNIQUE_##TYPE ( LIST_##TYPE *self, int (*less_than) (TYPE, TYPE))  \
  {                                                                                           \
    if (LIST_SIZE (self) < 2)                                                                 \
      return 0;                                                                               \
    if (!less_than)                                                                           \
      less_than = self->LessThan;                                                             \
                                                                                              \
    size_t ret = 0;                                                                           \
    LNODE(TYPE) *next;                                                                        \
    for (LNODE(TYPE) *node = LIST_BEGIN(self) ; node != LIST_END(self) && (next = BNODE_NEXT(node)) != LIST_END(self) ;)       \
        if (BNODE_CMP_VALUE___list_dummy___##TYPE(&node, &next, &less_than) == 0)             \
        {                                                                                     \
          ret++;                                                                              \
          LIST_REMOVE (self, next);                                                           \
          next = BNODE_NEXT(node);                                                            \
        }                                                                                     \
        else                                                                                  \
          node = next;                                                                        \
                                                                                              \
    return ret;                                                                               \
  }                                                                                           \
\
  static void LIST_SORT_##TYPE ( LIST_##TYPE *self, int (*less_than) (TYPE, TYPE))            \
  {                                                                                           \
    if (LIST_SIZE (self) < 2)                                                                 \
      return;                                                                                 \
                                                                                              \
    LNODE(TYPE) **array = malloc (LIST_SIZE (self) * sizeof (*array));                        \
    if (!array)                                                                               \
      return;                                                                                 \
                                                                                              \
    size_t index = 0;                                                                         \
    for (LNODE(TYPE) *node = LIST_BEGIN(self) ; node != LIST_END(self) ; node = BNODE_NEXT(node)) \
    {                                                                                         \
      array[index] = node;                                                                    \
      index++;                                                                                \
    }                                                                                         \
                                                                                              \
    if (!less_than)                                                                           \
      less_than = self->LessThan;                                                             \
    qsort_r (array, index, sizeof (*array), BNODE_CMP_VALUE___list_dummy___##TYPE, &less_than);   \
                                                                                              \
    for (size_t i = 0 ; i < index ; i++)                                                      \
      LIST_MOVE (self, LIST_END (self), self, array[i]);                                      \
                                                                                              \
    free (array);                                                                             \
  }                                                                                           \
\
  static int LIST_SWAP_##TYPE ( LIST_##TYPE *la, BNODE___list_dummy___##TYPE *nodea, LIST_##TYPE *lb, BNODE___list_dummy___##TYPE *nodeb )     \
  {                                                                    \
    if (nodea == nodeb)                                                \
      return EXIT_SUCCESS;                                             \
                                                                       \
    BNODE___list_dummy___##TYPE *nextb = BNODE_NEXT (nodeb);           \
                                                                       \
    if (LIST_MOVE (la, nodea, lb, nodeb) == EXIT_FAILURE ||            \
        LIST_MOVE (lb, nextb, la, nodea) == EXIT_FAILURE)              \
      return EXIT_FAILURE;                                             \
                                                                       \
    return EXIT_SUCCESS;                                               \
  }                                                                    \
\
  static int LIST_REVERSE_##TYPE ( LIST_##TYPE *self, BNODE___list_dummy___##TYPE *nodea, BNODE___list_dummy___##TYPE *nodeb )  \
  {                                                                        \
    if (nodea == nodeb)                                                    \
      return EXIT_SUCCESS;                                                 \
                                                                           \
    BNODE___list_dummy___##TYPE *n;                                        \
    BNODE___list_dummy___##TYPE *to = nodeb;                               \
    for (BNODE___list_dummy___##TYPE *from = nodea ; from && BNODE_NEXT (from) != to ; to = from, from = n)  \
    {                                                                      \
      n = BNODE_NEXT ( from );                                             \
      if (LIST_MOVE (self, to, self, from) == EXIT_FAILURE)                \
        return EXIT_FAILURE;                                               \
    }                                                                      \
                                                                           \
    return EXIT_SUCCESS;                                                   \
  }                                                                        \
\
  static BNODE___list_dummy___##TYPE *LIST_END_##TYPE ( LIST_##TYPE *self ) \
  {                                                                        \
    (void)self;                                                            \
    return 0;                                                              \
  }                                                                        \

#endif
