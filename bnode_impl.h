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

#ifndef __BNODE_IMPL_H__
#define __BNODE_IMPL_H__

#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include "bnode.h"

#define DEFINE_BNODE( K, T )  \
\
  static BNODE_##K##_##T *BNODE_COPY_##K##_##T( BNODE_##K##_##T *self );                             \
  static void BNODE_DESTROY_##K##_##T( BNODE_##K##_##T *self );                                      \
  static BNODE_##K##_##T *BNODE_REMOVE_##K##_##T( BNODE_##K##_##T *self );                           \
  static int BNODE_LESS_THAN_KEY_##K##_##T( K a, K b, int (*lt)(K, K) );                             \
  static int BNODE_CMP_KEY_##K##_##T(const void *a, const void *b, void* arg);                       \
  static int BNODE_LESS_THAN_VALUE_##K##_##T( T a, T b, int (*lt)(T, T) );                           \
  static int BNODE_CMP_VALUE_##K##_##T(const void *a, const void *b, void* arg);                     \
  static BNODE_##K##_##T * BNODE_FOR_EACH_##K##_##T( BNODE_##K##_##T *begin, BNODE_##K##_##T *end,   \
                                             int (*callback)( BNODE_##K##_##T *, void * ),           \
                                             void *param, int stop_condition, BNODE_DIRECTION direction ); \
  static BNODE_##K##_##T * BNODE_ASSIGN_##K##_##T ( BNODE_##K##_##T *self, T value );                \
  static BNODE_##K##_##T * BNODE_FIND_KEY_##K##_##T( BNODE_##K##_##T *begin, BNODE_##K##_##T *end,   \
                                         K key, int (*lt)(K, K), BNODE_DIRECTION direction );        \
  static BNODE_##K##_##T * BNODE_FIND_VALUE_##K##_##T( BNODE_##K##_##T *begin, BNODE_##K##_##T *end, \
                                         T value, int (*lt)(T, T), BNODE_DIRECTION direction );      \
  static BNODE_##K##_##T * BNODE_PREVIOUS_##K##_##T( BNODE_##K##_##T *node );                        \
  static BNODE_##K##_##T * BNODE_NEXT_##K##_##T( BNODE_##K##_##T *node );                            \
  static int BNODE_TREE_ADD_##K##_##T( BNODE_##K##_##T *self, BNODE_##K##_##T *newNode,              \
                                                     int (*lt)(K, K), int keep_subtree);             \
  static int BNODE_TREE_INSERT_BEFORE_##K##_##T( BNODE_##K##_##T *self, BNODE_##K##_##T *here,       \
                                                  BNODE_##K##_##T *newNode);                         \
  static BNODE_##K##_##T * BNODE_INDEX_##K##_##T ( BNODE_##K##_##T *self, size_t index );            \
  static BNODE_##K##_##T * BNODE_KEY_##K##_##T (BNODE_##K##_##T *self, K key, int (*lt)(K, K));      \
  static int BNODE_TRAVERSE_##K##_##T( BNODE_##K##_##T *tree,                                        \
                                       int (*callback)( BNODE_##K##_##T *, void * ),                 \
                                       void *param, int stop_condition );                            \
\
  static const _BNODE_VTABLE_##K##_##T BNODE_VTABLE_##K##_##T = \
  {                                                             \
    BNODE_COPY_##K##_##T,                                       \
    BNODE_DESTROY_##K##_##T,                                    \
    BNODE_REMOVE_##K##_##T,                                     \
    BNODE_LESS_THAN_KEY_##K##_##T,                              \
    BNODE_CMP_KEY_##K##_##T,                                    \
    BNODE_LESS_THAN_VALUE_##K##_##T,                            \
    BNODE_CMP_VALUE_##K##_##T,                                  \
    BNODE_FOR_EACH_##K##_##T,                                   \
    BNODE_ASSIGN_##K##_##T,                                     \
    BNODE_FIND_KEY_##K##_##T,                                   \
    BNODE_FIND_VALUE_##K##_##T,                                 \
    BNODE_PREVIOUS_##K##_##T,                                   \
    BNODE_NEXT_##K##_##T,                                       \
    BNODE_TREE_ADD_##K##_##T,                                   \
    BNODE_TREE_INSERT_BEFORE_##K##_##T,                         \
    BNODE_INDEX_##K##_##T,                                      \
    BNODE_KEY_##K##_##T,                                        \
    BNODE_TRAVERSE_##K##_##T,                                   \
  };                                                            \
\
  BNODE_##K##_##T *BNODE_CREATE_##K##_##T( K key, int unique )  \
  {                                                             \
    BNODE_##K##_##T *newNode = malloc( sizeof( *newNode ) );    \
    if (!newNode)                                               \
      return 0;                                                 \
    newNode->key = COPY_##K ? COPY_##K(key) : key;              \
    static T __zero__;                                          \
    newNode->data = __zero__;                                   \
    newNode->vtable = &BNODE_VTABLE_##K##_##T;                  \
    newNode->parent = 0;                                        \
    newNode->lower_child = 0;                                   \
    newNode->higher_child = 0;                                  \
    newNode->lowest_child = newNode;                            \
    newNode->highest_child = newNode;                           \
    newNode->size = newNode->depth = 1;                         \
    newNode->unique = unique;                                   \
    return newNode;                                             \
  }                                                             \
\
  static BNODE_##K##_##T *BNODE_COPY_##K##_##T( BNODE_##K##_##T *self )        \
  {                                                                            \
    BNODE_##K##_##T *ret = BNODE_CREATE_##K##_##T (self->key, self->unique);   \
    ret->data = COPY_##T ? COPY_##T(self->data) : self->data;                  \
    return ret;                                                                \
  }                                                                            \
\
  static void BNODE_DESTROY_##K##_##T( BNODE_##K##_##T *node ) \
  {                                                            \
    if (node->parent)                                          \
    {                                                          \
      errno = EINVAL;                                          \
      return;                                                  \
    }                                                          \
                                                               \
    if (node->lower_child)                                     \
    {                                                          \
      node->lower_child->parent = 0;                           \
      BNODE_DESTROY_##K##_##T(node->lower_child);              \
    }                                                          \
                                                               \
    if (node->higher_child)                                    \
    {                                                          \
      node->higher_child->parent = 0;                          \
      BNODE_DESTROY_##K##_##T(node->higher_child);             \
    }                                                          \
                                                               \
    if(DESTROY_##K)                                            \
      DESTROY_##K( node->key );                                \
    if(DESTROY_##T)                                            \
      DESTROY_##T( node->data );                               \
                                                               \
    free( node );                                              \
  }                                                            \
\
  static int BNODE_LESS_THAN_KEY_##K##_##T( K a, K b, int (*lt)(K, K) )   \
  {                                                               \
    return lt ?                                                   \
             lt (a,   b) :                                        \
             LESS_THAN_##K ?                                      \
             LESS_THAN_##K (a,   b) :                             \
             (size_t)0 != (size_t)(LESS_THAN_DEFAULT (K, a, b)) ? \
             LESS_THAN_DEFAULT (K, a, b)(a,   b) :                \
             (fprintf (stderr, "%s", "ERROR: " "Missing less than operator for type '" #K "'.\n"          \
                                     "       " "Use SET_LESS_THAN_OPERATOR(" #K ", operator),\n"          \
                                     "       " "where operator is a function defined as:\n"               \
                                     "       " "int operator(" #K " a, " #K " b) { return a < b ; }.\n"   \
                                     "ABORT  " "\n"), fflush (0), raise (SIGABRT));                       \
  }                                                                                                       \
\
  static int BNODE_LESS_THAN_VALUE_##K##_##T( T a, T b, int (*lt)(T, T) ) \
  {                                                               \
    return lt ?                                                   \
             lt (a,   b) :                                        \
             LESS_THAN_##T ?                                      \
             LESS_THAN_##T (a,   b) :                             \
             (size_t)0 != (size_t)(LESS_THAN_DEFAULT (T, a, b)) ? \
             LESS_THAN_DEFAULT (T, a, b)(a,   b) :                \
             (fprintf (stderr, "%s", "ERROR: " "Missing less than operator for type '" #T "'.\n"          \
                                     "       " "Use SET_LESS_THAN_OPERATOR(" #T ", operator),\n"          \
                                     "       " "where operator is a function defined as:\n"               \
                                     "       " "int operator(" #T " a, " #T " b) { return a < b ; }.\n"   \
                                     "ABORT  " "\n"), fflush (0), raise (SIGABRT));                       \
  }                                                                                                       \
\
  static int BNODE_CMP_KEY_##K##_##T(const void *a, const void *b, void* arg) \
  {                                                                                     \
    BNODE_##K##_##T *na = *(BNODE_##K##_##T **)a;                                       \
    BNODE_##K##_##T *nb = *(BNODE_##K##_##T **)b;                                       \
    int (*less_than) (K, K) = 0;                                                        \
    if (arg)                                                                            \
      less_than = *(int (**) (K, K))arg;                                                \
    return BNODE_LESS_THAN_KEY_##K##_##T (*BNODE_KEY (nb), *BNODE_KEY (na), less_than) ?  1 :    \
           BNODE_LESS_THAN_KEY_##K##_##T (*BNODE_KEY (na), *BNODE_KEY (nb), less_than) ? -1 : 0; \
  }                                                                                     \
\
  static int BNODE_CMP_VALUE_##K##_##T(const void *a, const void *b, void* arg)               \
  {                                                                                           \
    BNODE_##K##_##T *na = *(BNODE_##K##_##T **)a;                                             \
    BNODE_##K##_##T *nb = *(BNODE_##K##_##T **)b;                                             \
    int (*less_than) (T, T) = 0;                                                              \
    if (arg)                                                                                  \
      less_than = *(int (**) (T, T))arg;                                                      \
    return BNODE_LESS_THAN_VALUE_##K##_##T (*BNODE_VALUE (nb), *BNODE_VALUE (na), less_than) ?  1 :    \
           BNODE_LESS_THAN_VALUE_##K##_##T (*BNODE_VALUE (na), *BNODE_VALUE (nb), less_than) ? -1 : 0; \
  }                                                                                           \
\
  static BNODE_##K##_##T * BNODE_FOR_EACH_##K##_##T( BNODE_##K##_##T *begin, BNODE_##K##_##T *end,              \
                                             int (*callback)( BNODE_##K##_##T *, void * ),                      \
                                             void *param, int stop_condition, BNODE_DIRECTION direction )       \
  {                                                                                                             \
    int ret;                                                                                                    \
                                                                                                                \
    if (!callback)                                                                                              \
      return 0;                                                                                                 \
                                                                                                                \
    BNODE_##K##_##T *scan = begin, *temp = 0;                                                                   \
                                                                                                                \
    while(scan && scan != end)                                                                                  \
    {                                                                                                           \
      temp = direction == BNODE_FORWARD ? BNODE_NEXT_##K##_##T (scan) : BNODE_PREVIOUS_##K##_##T (scan);        \
                                                                                                                \
      /* Stop the callback process if return type is not RETURN_SUCCESS */                                      \
      /* Returns the last processed element */                                                                  \
      if((ret = callback( scan, param )) == stop_condition)                                                     \
        return scan;                                                                                            \
                                                                                                                \
      scan = temp;                                                                                              \
    }                                                                                                           \
                                                                                                                \
    return 0;                                                                                                   \
  }                                                                                                             \
\
  static int BNODE_TRAVERSE_##K##_##T( BNODE_##K##_##T *tree,                                     \
                                       int (*callback)( BNODE_##K##_##T *, void * ),              \
                                       void *param, int stop_condition )                          \
  {                                                                                               \
    int ret;                                                                                      \
                                                                                                  \
    if (!callback)                                                                                \
      return EXIT_FAILURE;                                                                        \
                                                                                                  \
    BNODE_##K##_##T *lc = tree->lower_child;                                                      \
    BNODE_##K##_##T *hc = tree->higher_child;                                                     \
                                                                                                  \
    if (lc &&                                                                                     \
        BNODE_TRAVERSE_##K##_##T (lc, callback, param, stop_condition) == stop_condition)         \
      return stop_condition;                                                                      \
                                                                                                  \
    if ((ret = callback(tree, param)) == stop_condition)                                          \
      return stop_condition;                                                                      \
                                                                                                  \
    if (hc &&                                                                                     \
        BNODE_TRAVERSE_##K##_##T (hc, callback, param, stop_condition) == stop_condition)         \
      return stop_condition;                                                                      \
                                                                                                  \
    return ret;                                                                                   \
  }                                                                                               \
\
  static BNODE_##K##_##T * BNODE_FIND_KEY_##K##_##T( BNODE_##K##_##T *begin, BNODE_##K##_##T *end,              \
                                         K key, int (*lt)(K, K), BNODE_DIRECTION direction )                    \
  {                                                                                                             \
    BNODE_##K##_##T *scan = begin, *temp = 0;                                                                   \
                                                                                                                \
    while(scan && scan != end)                                                                                  \
    {                                                                                                           \
      temp = direction == BNODE_FORWARD ? BNODE_NEXT_##K##_##T (scan) : BNODE_PREVIOUS_##K##_##T (scan);        \
                                                                                                                \
      /* Stop the process if return type is RETURN_SUCCESS */                                                   \
      /* Returns the last processed element */                                                                  \
      if (!BNODE_LESS_THAN_KEY_##K##_##T (scan->key, key, lt) &&                                                \
          !BNODE_LESS_THAN_KEY_##K##_##T (key, scan->key, lt))                                                  \
        return scan;                                                                                            \
                                                                                                                \
      scan = temp;                                                                                              \
    }                                                                                                           \
                                                                                                                \
    return 0;                                                                                                   \
  }                                                                                                             \
\
  static BNODE_##K##_##T * BNODE_FIND_VALUE_##K##_##T( BNODE_##K##_##T *begin, BNODE_##K##_##T *end,            \
                                         T value, int (*lt)(T, T), BNODE_DIRECTION direction )                  \
  {                                                                                                             \
    BNODE_##K##_##T *scan = begin, *temp = 0;                                                                   \
                                                                                                                \
    while(scan && scan != end)                                                                                  \
    {                                                                                                           \
      temp = direction == BNODE_FORWARD ? BNODE_NEXT_##K##_##T (scan) : BNODE_PREVIOUS_##K##_##T (scan);        \
                                                                                                                \
      /* Stop the process if return type is RETURN_SUCCESS */                                                   \
      /* Returns the last processed element */                                                                  \
      if (!BNODE_LESS_THAN_VALUE_##K##_##T (scan->data, value, lt) &&                                           \
          !BNODE_LESS_THAN_VALUE_##K##_##T (value, scan->data, lt))                                             \
        return scan;                                                                                            \
                                                                                                                \
      scan = temp;                                                                                              \
    }                                                                                                           \
                                                                                                                \
    return 0;                                                                                                   \
  }                                                                                                             \
\
  static BNODE_##K##_##T * BNODE_ASSIGN_##K##_##T ( BNODE_##K##_##T *node, T data )     \
  {                                                                \
    if (node)                                                      \
    {                                                              \
      if (DESTROY_##T)                                             \
        DESTROY_##T( node->data );                                 \
      node->data = COPY_##T ? COPY_##T(data) : data;               \
    }                                                              \
    return node;                                                   \
  }                                                                \
\
  static BNODE_##K##_##T * BNODE_PREVIOUS_##K##_##T( BNODE_##K##_##T *self )    \
  {                                                                         \
    if (self->lower_child)                                                  \
    {                                                                       \
      /* Any element below self->lower_child is lower than self.  */        \
      /* We look for the highest descendant of self->lower_child. */        \
      BNODE_##K##_##T * ret = self->lower_child;                            \
      while (ret->higher_child)                                             \
        ret = ret->higher_child;                                            \
      return ret;                                                           \
    }                                                                       \
    else                                                                    \
    {                                                                       \
      BNODE_##K##_##T * ret = self;                                         \
      while (ret->parent && ret->parent->lower_child == ret)                \
           ret = ret->parent;                                               \
      return ret->parent;                                                   \
    }                                                                       \
  }                                                                         \
\
  static BNODE_##K##_##T * BNODE_NEXT_##K##_##T( BNODE_##K##_##T *self )    \
  {                                                                         \
    if (self->higher_child)                                                 \
    {                                                                       \
      /* Any element below self->higher_child is higher than self. */       \
      /* We look for the lowest descendant of self->higher_child.  */       \
      BNODE_##K##_##T * ret = self->higher_child;                           \
      while (ret->lower_child)                                              \
        ret = ret->lower_child;                                             \
      return ret;                                                           \
    }                                                                       \
    else                                                                    \
    {                                                                       \
      BNODE_##K##_##T * ret = self;                                         \
      while (ret->parent && ret->parent->higher_child == ret)               \
        ret = ret->parent;                                                  \
      return ret->parent;                                                   \
    }                                                                       \
  }                                                                         \
\
  static int BNODE_TREE_INSERT_BEFORE_##K##_##T(BNODE_##K##_##T *self,      \
                                                BNODE_##K##_##T *here,      \
                                                BNODE_##K##_##T *newNode)   \
  {                                                                         \
    if (newNode->parent || newNode == self)                                 \
    {                                                                       \
      errno = EINVAL;                                                       \
      return EXIT_FAILURE;                                                  \
    }                                                                       \
                                                                            \
    /* Go to root */                                                        \
    BNODE_##K##_##T *n;                                                     \
    for (n = here ; n && n->parent ; n = n->parent) /* nop */ ;             \
    /* Check that herefrom is owned by from */                              \
    if (here && n != self)                                                  \
    {                                                                       \
      errno = EINVAL;                                                       \
      return EXIT_FAILURE;                                                  \
    }                                                                       \
                                                                            \
    /* parent, higher_child, lower_child                              */    \
    if (!here)                                                              \
    {                                                                       \
      self->highest_child->higher_child = newNode;                          \
      newNode->parent = self->highest_child;                                \
    }                                                                       \
    else if (!here->lower_child)                                            \
    {                                                                       \
      here->lower_child = newNode;                                          \
      newNode->parent = here;                                               \
    }                                                                       \
    else                                                                    \
    {                                                                       \
      here->lower_child->highest_child->higher_child = newNode;             \
      newNode->parent = here->lower_child->highest_child;                   \
    }                                                                       \
                                                                            \
    /* highest_child, lowest_child                                    */    \
    /* depth, size                                                    */    \
    for (BNODE_##K##_##T *p = newNode->parent ; p ; p = p->parent)          \
    {                                                                       \
      p->size += newNode->size;                                           \
      if (!p->lower_child)                                                \
      {                                                                   \
        p->depth = p->higher_child->depth + 1;                            \
        p->highest_child = p->higher_child->highest_child;                \
        p->lowest_child = p;                                              \
      }                                                                   \
      else if (!p->higher_child)                                          \
      {                                                                   \
        p->depth = p->lower_child->depth + 1;                             \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p;                                             \
      }                                                                   \
      else if (p->lower_child->depth < p->higher_child->depth)            \
      {                                                                   \
        p->depth = p->higher_child->depth + 1;                            \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p->higher_child->highest_child;                \
      }                                                                   \
      else                                                                \
      {                                                                   \
        p->depth = p->lower_child->depth + 1;                             \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p->higher_child->highest_child;                \
      }                                                                   \
    }                                                                     \
                                                                          \
    return EXIT_SUCCESS;                                                  \
  }                                                                       \
\
  static int BNODE_IS_LOWER_##K##_##T(K a, K b) {(void)a ; (void)b ; return 0;}                 \
  static int BNODE_IS_HIGHER_##K##_##T(K a, K b) {(void)a ; (void)b ; return 0;}                \
\
  static int BNODE_TREE_ADD_##K##_##T(BNODE_##K##_##T *self,                \
                                      BNODE_##K##_##T *newNode,             \
                                      int (*lt)(K, K), int keep_subtree)    \
  {                                                                         \
    if (newNode->parent || newNode == self)                                 \
    {                                                                       \
      errno = EINVAL;                                                       \
      return 0;                                                             \
    }                                                                       \
                                                                            \
    BNODE_##K##_##T *lower = newNode->lower_child;                          \
    BNODE_##K##_##T *higher = newNode->higher_child;                        \
    if (!keep_subtree)  /* newNode isolation */                             \
    {                                                                       \
      newNode->lower_child = newNode->higher_child = 0;                     \
      newNode->lowest_child = newNode->highest_child = newNode;             \
      newNode->size = newNode->depth = 1;                                   \
    }                                                                       \
                                                                            \
    /* Add newNode in the tree */                                           \
    int ret = 0;                                                            \
    int comp;                                                               \
    if (lt == BNODE_IS_HIGHER_##K##_##T)                                    \
      comp = 1;                                                             \
    else if (lt == BNODE_IS_LOWER_##K##_##T)                                \
      comp = -1;                                                            \
    else                                                                    \
      comp = BNODE_CMP_KEY_##K##_##T (&newNode, &self, &lt);                \
    if (comp > 0)                                                           \
    {                                                                       \
      if (self->higher_child)                                               \
        ret = BNODE_TREE_ADD_##K##_##T (self->higher_child, newNode, lt, keep_subtree);   \
      else                                                                  \
      {                                                                     \
        newNode->parent = self;                                             \
        self->higher_child = newNode;                                       \
        ret = newNode->size;                                                \
      }                                                                     \
      if (ret)                                                              \
      {                                                                     \
        self->size += ret;                                                  \
        if (!self->lower_child || self->lower_child->depth < self->higher_child->depth + 1) \
          self->depth = self->higher_child->depth + 1;                      \
        self->highest_child = self->higher_child->highest_child;            \
      }                                                                     \
    }                                                                       \
    else if (comp == 0 && self->unique)                                     \
    {                                                                       \
      assert (!keep_subtree);                                               \
      BNODE_DESTROY_##K##_##T (newNode);                                    \
      ret = 0;                                                              \
    }                                                                       \
    else /* comp <= 0 && (comp != 0 || !self->unique)                    */ \
    {                                                                       \
      if (self->lower_child)                                                \
        ret = BNODE_TREE_ADD_##K##_##T (self->lower_child, newNode, lt, keep_subtree);    \
      else                                                                  \
      {                                                                     \
        newNode->parent = self;                                             \
        self->lower_child = newNode;                                        \
        ret = newNode->size;                                                \
      }                                                                     \
      if (ret)                                                              \
      {                                                                     \
        self->size += ret;                                                  \
        if (!self->higher_child || self->higher_child->depth < self->lower_child->depth + 1) \
          self->depth = self->lower_child->depth + 1;                       \
        self->lowest_child = self->lower_child->lowest_child;               \
      }                                                                     \
    }                                                                       \
                                                                            \
    if (!keep_subtree)                                                      \
    {                                                                       \
      /* Add leaves of newNode in the tree */                               \
      /* (allow to add a tree in a tree) */                                 \
      if (lower)                                                            \
      {                                                                     \
        lower->parent = 0;                                                  \
        ret += BNODE_TREE_ADD_##K##_##T(self, lower, lt, keep_subtree);     \
      }                                                                     \
      if (higher)                                                           \
      {                                                                     \
        higher->parent = 0;                                                 \
        ret += BNODE_TREE_ADD_##K##_##T(self, higher, lt, keep_subtree);    \
      }                                                                     \
    }                                                                       \
    else if (0)  /* TODO: check this */                                     \
      ret += (lower ? lower->size : 0) + (higher ? higher->size : 0);       \
                                                                            \
    return ret;                                                             \
  }                                                                         \
\
  static BNODE_##K##_##T *BNODE_REMOVE_##K##_##T( BNODE_##K##_##T *node ) \
  {                                                                       \
    int is_lower = node->parent && node->parent->lower_child == node;     \
    int is_higher = node->parent && node->parent->higher_child == node;   \
                                                                          \
    BNODE_##K##_##T *ret = 0;                                             \
    if (node->lower_child && node->higher_child)                          \
    {                                                                     \
      if (node->lower_child->depth < node->higher_child->depth)           \
      {                                                                   \
        node->higher_child->parent = 0;                                   \
        BNODE_TREE_ADD_##K##_##T(ret = node->lower_child, node->higher_child, BNODE_IS_HIGHER_##K##_##T, 1); \
      }                                                                   \
      else if (node->lower_child->depth >= node->higher_child->depth)     \
      {                                                                   \
        node->lower_child->parent = 0;                                    \
        BNODE_TREE_ADD_##K##_##T(ret = node->higher_child, node->lower_child, BNODE_IS_LOWER_##K##_##T, 1); \
      }                                                                   \
    }                                                                     \
    else if (node->lower_child)                                           \
      ret = node->lower_child;                                            \
    else if (node->higher_child)                                          \
      ret = node->higher_child;                                           \
    else                                                                  \
      ret = 0;                                                            \
                                                                          \
    if (is_lower)                                                         \
      node->parent->lower_child = ret;                                    \
    else if (is_higher)                                                   \
      node->parent->higher_child = ret;                                   \
                                                                          \
    if (ret)                                                              \
      ret->parent = node->parent;                                         \
                                                                          \
    for (BNODE_##K##_##T *p = node->parent ; p ; p = p->parent)           \
    {                                                                     \
      p->size--;                                                          \
      if (!p->lower_child && !p->higher_child)                            \
      {                                                                   \
        p->depth = 1;                                                     \
        p->lowest_child = p->highest_child = p;                           \
      }                                                                   \
      else if (!p->lower_child)                                           \
      {                                                                   \
        p->depth = p->higher_child->depth + 1;                            \
        p->highest_child = p->higher_child->highest_child;                \
        p->lowest_child = p;                                              \
      }                                                                   \
      else if (!p->higher_child)                                          \
      {                                                                   \
        p->depth = p->lower_child->depth + 1;                             \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p;                                             \
      }                                                                   \
      else if (p->lower_child->depth < p->higher_child->depth)            \
      {                                                                   \
        p->depth = p->higher_child->depth + 1;                            \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p->higher_child->highest_child;                \
      }                                                                   \
      else                                                                \
      {                                                                   \
        p->depth = p->lower_child->depth + 1;                             \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p->higher_child->highest_child;                \
      }                                                                   \
    }                                                                     \
                                                                          \
    node->parent = 0;                                                     \
    node->lower_child = node->higher_child = 0;                           \
    node->lowest_child = node->highest_child = node;                      \
    node->depth = node->size = 1;                                         \
                                                                          \
    return ret;                                                           \
  }                                                                       \
\
  static BNODE_##K##_##T *BNODE_INDEX_##K##_##T (BNODE_##K##_##T *self, size_t index) \
  {                                                                               \
    if (index >= self->size)                                                      \
    {                                                                             \
      fprintf (stderr, "ERROR: " "Index %zi out of range [0, %zi].\n"             \
                       "ABORT  " "\n", index, self->size - 1);                    \
      fflush (0) ; raise (SIGABRT);                                               \
      return 0;                                                                   \
    }                                                                             \
    if (self->lower_child && index < self->lower_child->size)                     \
      return BNODE_INDEX_##K##_##T (self->lower_child, index);                    \
    if (self->higher_child && index > (self->lower_child ? self->lower_child->size : 0))  \
      return BNODE_INDEX_##K##_##T (self->higher_child, index - 1 - (self->lower_child ? self->lower_child->size : 0));  \
    return self;                                                                  \
  }                                                                               \
\
  static BNODE_##K##_##T *BNODE_KEY_##K##_##T (BNODE_##K##_##T *self, K key, int (*lt)(K, K)) \
  {                                                                               \
    if (BNODE_LESS_THAN_KEY_##K##_##T(key, self->key, lt))                        \
    {                                                                             \
      if (self->lower_child)                                                      \
        return BNODE_KEY_##K##_##T (self->lower_child, key, lt);                  \
    }                                                                             \
    else if (BNODE_LESS_THAN_KEY_##K##_##T(self->key, key, lt))                   \
    {                                                                             \
      if (self->higher_child)                                                     \
        return BNODE_KEY_##K##_##T (self->higher_child, key, lt);                 \
    }                                                                             \
    else                                                                          \
      return self;                                                                \
                                                                                  \
    return 0;                                                                     \
  }                                                                               \
  
#endif
