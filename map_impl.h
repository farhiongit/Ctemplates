/*******
 * Copyright 2017 Laurent Farhi
 *
 *  This file is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This file is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
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

#ifndef __MAP_IMPL_H__
#define __MAP_IMPL_H__

#define _GNU_SOURCE
#include <stdlib.h>
#include <errno.h>
#include "bnode_impl.h"
#include "map.h"
#include "defops.h"

//
// DEFINE_MAP
// Purpose: Defines a linked list of a specific type on the heap.
//
#define DEFINE_MAP( K, T )                              \
\
  DEFINE_BNODE(K, T);                                   \
\
  static void MAP_CLEAR_##K##_##T ( MAP_##K##_##T *self );                                             \
  static int MAP_REMOVE_##K##_##T ( MAP_##K##_##T *self, BNODE_##K##_##T *node );                      \
  static int MAP_MOVE_##K##_##T ( MAP_##K##_##T *to, MAP_##K##_##T *from, BNODE_##K##_##T *herefrom ); \
  static BNODE_##K##_##T *MAP_SET_##K##_##T ( MAP_##K##_##T *self, K key, T value );                   \
  static BNODE_##K##_##T *MAP_END_##K##_##T ( MAP_##K##_##T *self );                                   \
\
  static BNODE_##K##_##T MAP_NULL_##K##_##T = { .vtable = &BNODE_VTABLE_##K##_##T };                  \
\
  static const _MAP_VTABLE_##K##_##T MAP_VTABLE_##K##_##T =   \
  {                                                        \
    BNODE_CREATE_##K##_##T,                                \
    MAP_CLEAR_##K##_##T,                                   \
    MAP_REMOVE_##K##_##T,                                  \
    MAP_MOVE_##K##_##T,                                    \
    MAP_SET_##K##_##T,                                     \
    MAP_END_##K##_##T,                                     \
  };                                                       \
\
  MAP_##K##_##T *MAP_CREATE_##K##_##T ( int (*less_than_operator) (K, K), int unique )        \
  {                                                                          \
    MAP_##K##_##T *linkedList = malloc( sizeof( *linkedList ) );             \
    if (!linkedList)                                                         \
      return 0;                                                              \
                                                                             \
    linkedList->vtable = &MAP_VTABLE_##K##_##T;                              \
    linkedList->LessThanKey = less_than_operator;                            \
    linkedList->LessThanValue = 0;                                           \
    linkedList->unique = unique;                                             \
    linkedList->tree_locked = 0;                                             \
                                                                             \
    linkedList->root = 0;                                                    \
    linkedList->null = &MAP_NULL_##K##_##T;                                  \
                                                                             \
    return linkedList;                                                       \
  }                                                                          \
\
  static BNODE_##K##_##T *MAP_INSERT_##K##_##T ( MAP_##K##_##T *self, BNODE_##K##_##T *node )  \
  {                                                                        \
    if (node->parent)                                                      \
    {                                                                      \
      errno = EINVAL;                                                      \
      return 0;                                                            \
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
    return !self->root ?                                                   \
             self->root = node :                                           \
             BNODE_TREE_ADD_##K##_##T(&(self->root), node, self->LessThanKey) ?     \
               node : 0;                                                   \
  }                                                                        \
\
  static BNODE_##K##_##T *MAP_SET_##K##_##T ( MAP_##K##_##T *self, K key, T value ) \
  {                                                                        \
    BNODE_##K##_##T * ret = 0;                                             \
    if (!self->root || !self->unique ||                                    \
        !(ret = BNODE_GET_KEY(self->root, key, self->LessThanKey)))        \
      ret = MAP_INSERT_##K##_##T (self, BNODE_CREATE_##K##_##T (key, self->unique));\
    BNODE_ASSIGN_##K##_##T (ret, value);                                   \
                                                                           \
    return ret;                                                            \
  }                                                                        \
\
  static int MAP_REMOVE_##K##_##T ( MAP_##K##_##T *self, BNODE_##K##_##T *node )   \
  {                                                                        \
    /* Go to root */                                                       \
    BNODE_##K##_##T *n;                                                    \
    for (n = node ; n->parent ; n = n->parent) /* nop */ ;                 \
    /* Check that node is owned by self */                                 \
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
    BNODE_##K##_##T *new_root = BNODE_REMOVE_##K##_##T(node);              \
    if (self->root == node)                                                \
      self->root = new_root;                                               \
    BNODE_DESTROY_##K##_##T(node);                                         \
    return EXIT_SUCCESS;                                                   \
  }                                                                        \
\
  static int MAP_MOVE_##K##_##T ( MAP_##K##_##T *to, MAP_##K##_##T *from, BNODE_##K##_##T *herefrom )  \
  {                                                                        \
    if (to == from)                                                        \
      return EXIT_SUCCESS;                                                 \
                                                                           \
    /* Go to root */                                                       \
    BNODE_##K##_##T *n;                                                    \
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
    BNODE_##K##_##T *new_node = BNODE_REMOVE_##K##_##T(herefrom);          \
    if (from->root == herefrom)                                            \
      from->root = new_node;                                               \
    MAP_INSERT_##K##_##T (to, herefrom);                                   \
                                                                           \
    return EXIT_SUCCESS;                                                   \
  }                                                                        \
\
  static void MAP_CLEAR_##K##_##T ( MAP_##K##_##T *self )  \
  {                                               \
    if (self->root)                               \
      BNODE_DESTROY_##K##_##T(self->root);        \
    self->root = 0;                               \
  }                                               \
\
  static BNODE_##K##_##T *MAP_END_##K##_##T ( MAP_##K##_##T *self ) \
  {                                                             \
    (void)self;                                                 \
    return 0;                                                   \
  }                                                             \
  struct __useless_struct_MAP_IMPL_##K##_##T

#endif
