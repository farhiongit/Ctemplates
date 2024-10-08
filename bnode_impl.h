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
// https://en.wikipedia.org/wiki/Binary_search_tree
// https://en.wikipedia.org/wiki/AVL_tree
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
  static size_t BNODE_TREE_ADD_##K##_##T( BNODE_##K##_##T **self, BNODE_##K##_##T *newNode,          \
                                          int (*lt)(K, K) );                                         \
  static int BNODE_TREE_INSERT_BEFORE_##K##_##T( BNODE_##K##_##T **self, BNODE_##K##_##T *here,      \
                                                 BNODE_##K##_##T *newNode);                          \
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
    newNode->key = COPY_##K ? COPY_##K(key) : COPY_DEFAULT(K) ? COPY_DEFAULT(K)(key) : key ;\
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
  static void BNODE_RETRACE_##K##_##T( BNODE_##K##_##T *invalidated, BNODE_##K##_##T *until ) \
  {                                                                       \
    /* Retrace the tree up from the invalidated node */                   \
    /* Update attributes depth, size, lowest_child, highest_child */      \
    for (BNODE_##K##_##T *p = invalidated ; p && p != until ; p = p->parent) \
    {                                                                     \
      p->size = 1;                                                        \
      p->depth = 1;                                                       \
      if (!p->lower_child && !p->higher_child)                            \
      {                                                                   \
        p->lowest_child = p->highest_child = p;                           \
      }                                                                   \
      else if (!p->lower_child)                                           \
      {                                                                   \
        p->size += p->higher_child->size;                                 \
        p->depth += p->higher_child->depth;                               \
        p->highest_child = p->higher_child->highest_child;                \
        p->lowest_child = p;                                              \
      }                                                                   \
      else if (!p->higher_child)                                          \
      {                                                                   \
        p->size += p->lower_child->size;                                  \
        p->depth += p->lower_child->depth;                                \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p;                                             \
      }                                                                   \
      else if (p->lower_child->depth < p->higher_child->depth)            \
      {                                                                   \
        p->size += p->lower_child->size + p->higher_child->size;          \
        p->depth += p->higher_child->depth;                               \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p->higher_child->highest_child;                \
      }                                                                   \
      else                                                                \
      {                                                                   \
        p->size += p->lower_child->size + p->higher_child->size;          \
        p->depth += p->lower_child->depth;                                \
        p->lowest_child = p->lower_child->lowest_child;                   \
        p->highest_child = p->higher_child->highest_child;                \
      }                                                                   \
    }                                                                     \
  }                                                                       \
\
  static BNODE_##K##_##T *BNODE_ROTATE_LEFT_##K##_##T(BNODE_##K##_##T *A)  \
  {                                                                       \
    BNODE_##K##_##T *P = A->parent;                                       \
    BNODE_##K##_##T *B = A->higher_child;  /*   A                   */    \
    if (!B)                                /*  / \                  */    \
      return 0;                            /* e   B                 */    \
    BNODE_##K##_##T *C = B->lower_child;   /*    / \                */    \
    A->higher_child = C;                   /*   C   d =>   B        */    \
    if (C)                                 /*             / \       */    \
      C->parent = A;                       /*            A   d      */    \
    B->lower_child = A;                    /*           / \         */    \
    A->parent = B;                         /*          e   C        */    \
                                                                          \
    /* Update parent links */                                             \
    B->parent = P;                                                        \
    if (P)                                                                \
    {                                                                     \
      if (P->lower_child == A)                                            \
        P->lower_child = B;                                               \
      else /* if (P->higher_child == A) */                                \
        P->higher_child = B;                                              \
    }                                                                     \
                                                                          \
    /* Update depth, size, lowest_child, highest_child for A and B */     \
    BNODE_RETRACE_##K##_##T( A, B->parent );                              \
                                                                          \
    return B;                                                             \
  }                                                                       \
\
  static BNODE_##K##_##T *BNODE_ROTATE_RIGHT_##K##_##T(BNODE_##K##_##T *A)  \
  {                                                                       \
    BNODE_##K##_##T *P = A->parent;                                       \
    BNODE_##K##_##T *B = A->lower_child;   /*     A                 */    \
    if (!B)                                /*    / \                */    \
      return 0;                            /*   B   e               */    \
    BNODE_##K##_##T *C = B->higher_child;  /*  / \                  */    \
    A->lower_child = C;                    /* d   C   =>   B        */    \
    if (C)                                 /*             / \       */    \
      C->parent = A;                       /*            d   A      */    \
    B->higher_child = A;                   /*               / \     */    \
    A->parent = B;                         /*              C   e    */    \
                                                                          \
    /* Update parent links */                                             \
    B->parent = P;                                                        \
    if (P)                                                                \
    {                                                                     \
      if (P->lower_child == A)                                            \
        P->lower_child = B;                                               \
      else /* if (P->higher_child == A) */                                \
        P->higher_child = B;                                              \
    }                                                                     \
                                                                          \
    /* Update depth, size, lowest_child, highest_child for A and B */     \
    BNODE_RETRACE_##K##_##T( A, B->parent );                              \
                                                                          \
    return B;                                                             \
  }                                                                       \
\
  static BNODE_##K##_##T *BNODE_BALANCE_##K##_##T( BNODE_##K##_##T *from )\
  {                                                                       \
    /* Self-balancing the tree with a relaxed strategy.                */ \
    /* Balancing is asymetric: it applies only for branches longer     */ \
    /* than its sibling branch. It only shortens the branch containing */ \
    /* from and no other.                                              */ \
    BNODE_##K##_##T *new_root = 0;                                        \
                                                                          \
    for (BNODE_##K##_##T *n = from ; n && n->parent ; n = n->parent)      \
      if (n->parent->lower_child == n)                                    \
      {                                                                   \
        BNODE_##K##_##T *sibling = n->parent->higher_child;               \
        if (n->depth > (sibling ? sibling->depth + 1 : 1))                \
        {   /* balancing required */                                      \
          BNODE_##K##_##T *parent = n->parent;                            \
          BNODE_##K##_##T *grandparent = n->parent->parent;               \
          if (grandparent)                                                \
          {                                                               \
            if (grandparent->lower_child == parent)                       \
            {                                                             \
              grandparent->lower_child = BNODE_ROTATE_RIGHT_##K##_##T (parent);  \
              grandparent->lower_child->parent = grandparent;             \
            }                                                             \
            else /* if (grandparent->higher_child == parent) */           \
            {                                                             \
              grandparent->higher_child = BNODE_ROTATE_RIGHT_##K##_##T (parent); \
              grandparent->higher_child->parent = grandparent;            \
            }                                                             \
          }                                                               \
          else /* if (!grandparent) */                                    \
          {                                                               \
            new_root = BNODE_ROTATE_RIGHT_##K##_##T (parent);             \
            new_root->parent = 0;                                         \
          }                                                               \
        }                                                                 \
      }                                                                   \
      else /* if (n->parent->higher_child == n) */                        \
      {                                                                   \
        BNODE_##K##_##T *sibling = n->parent->lower_child;                \
        if (n->depth > (sibling ? sibling->depth + 1 : 1))                \
        {   /* balancing required */                                      \
          BNODE_##K##_##T *parent = n->parent;                            \
          BNODE_##K##_##T *grandparent = n->parent->parent;               \
          if (grandparent)                                                \
          {                                                               \
            if (grandparent->lower_child == parent)                       \
            {                                                             \
              grandparent->lower_child = BNODE_ROTATE_LEFT_##K##_##T (parent);   \
              grandparent->lower_child->parent = grandparent;             \
            }                                                             \
            else /* if (grandparent->higher_child == parent) */           \
            {                                                             \
              grandparent->higher_child = BNODE_ROTATE_LEFT_##K##_##T (parent);  \
              grandparent->higher_child->parent = grandparent;            \
            }                                                             \
          }                                                               \
          else /* if (!grandparent) */                                    \
          {                                                               \
            new_root = BNODE_ROTATE_LEFT_##K##_##T (parent);              \
            new_root->parent = 0;                                         \
          }                                                               \
        }                                                                 \
      }                                                                   \
                                                                          \
    return new_root;                                                      \
  }                                                                       \
\
  static BNODE_##K##_##T *BNODE_COPY_##K##_##T( BNODE_##K##_##T *self )        \
  {                                                                            \
    BNODE_##K##_##T *ret = BNODE_CREATE_##K##_##T (self->key, self->unique);   \
    ret->data = COPY_##T ? COPY_##T(self->data) : COPY_DEFAULT(T) ? COPY_DEFAULT(T)(self->data) : self->data ; \
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
    else if(DESTROY_DEFAULT(K))                                \
      DESTROY_DEFAULT(K)( node->key );                         \
                                                               \
    if(DESTROY_##T)                                            \
      DESTROY_##T( node->data );                               \
    else if(DESTROY_DEFAULT(T))                                \
      DESTROY_DEFAULT(T)( node->data );                        \
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
             (size_t)0 != (size_t)(LESS_THAN_DEFAULT (K)) ?       \
             LESS_THAN_DEFAULT (K)(a,   b) :                      \
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
             (size_t)0 != (size_t)(LESS_THAN_DEFAULT (T)) ?       \
             LESS_THAN_DEFAULT (T)(a,   b) :                      \
             (fprintf (stderr, "%s", "ERROR: " "Missing less than operator for type '" #T "'.\n"          \
                                     "       " "Use SET_LESS_THAN_OPERATOR(" #T ", operator),\n"          \
                                     "       " "where operator is a function defined as:\n"               \
                                     "       " "int operator(" #T " a, " #T " b) { return a < b ; }.\n"   \
                                     "ABORT  " "\n"), fflush (0), raise (SIGABRT));                       \
  }                                                                                                       \
\
  static int BNODE_CMP_KEY_##K##_##T(const void *a, const void *b, void* arg) \
  {                                                                                     \
    BNODE_##K##_##T *na = *(BNODE_##K##_##T * const *)a;                                \
    BNODE_##K##_##T *nb = *(BNODE_##K##_##T * const *)b;                                \
    int (*less_than) (K, K) = 0;                                                        \
    if (arg)                                                                            \
      less_than = *(int (**) (K, K))arg;                                                \
    return BNODE_LESS_THAN_KEY_##K##_##T (*BNODE_KEY (nb), *BNODE_KEY (na), less_than) ?  1 :    \
           BNODE_LESS_THAN_KEY_##K##_##T (*BNODE_KEY (na), *BNODE_KEY (nb), less_than) ? -1 : 0; \
  }                                                                                     \
\
  static int BNODE_CMP_VALUE_##K##_##T(const void *a, const void *b, void* arg)               \
  {                                                                                           \
    BNODE_##K##_##T *na = *(BNODE_##K##_##T * const *)a;                                      \
    BNODE_##K##_##T *nb = *(BNODE_##K##_##T * const *)b;                                      \
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
      else if (DESTROY_DEFAULT(T))                                 \
        DESTROY_DEFAULT(T)( node->data );                          \
      node->data = COPY_##T ? COPY_##T(data) : COPY_DEFAULT(T) ? COPY_DEFAULT(T)(data) : data;               \
    }                                                              \
    return node;                                                   \
  }                                                                \
\
  static BNODE_##K##_##T * BNODE_PREVIOUS_##K##_##T( BNODE_##K##_##T *self )\
  {                                                                         \
    if (self->lower_child)                                                  \
      return self->lower_child->highest_child;                              \
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
      return self->higher_child->lowest_child;                              \
    else                                                                    \
    {                                                                       \
      BNODE_##K##_##T * ret = self;                                         \
      while (ret->parent && ret->parent->higher_child == ret)               \
        ret = ret->parent;                                                  \
      return ret->parent;                                                   \
    }                                                                       \
  }                                                                         \
\
  static int BNODE_TREE_INSERT_BEFORE_##K##_##T(BNODE_##K##_##T **self,   \
                                                BNODE_##K##_##T *here,    \
                                                BNODE_##K##_##T *newNode) \
  {                                                                       \
    if (newNode->parent || newNode == *self)                              \
    {                                                                     \
      errno = EINVAL;                                                     \
      return EXIT_FAILURE;                                                \
    }                                                                     \
                                                                          \
    /* Go to root */                                                      \
    BNODE_##K##_##T *n;                                                   \
    for (n = here ; n && n->parent ; n = n->parent) /* nop */ ;           \
    /* Check that herefrom is owned by from */                            \
    if (here && n != *self)                                               \
    {                                                                     \
      errno = EINVAL;                                                     \
      return EXIT_FAILURE;                                                \
    }                                                                     \
                                                                          \
    /* parent, higher_child, lower_child                              */  \
    if (!here)                                                            \
    {                                                                     \
      (*self)->highest_child->higher_child = newNode;                     \
      newNode->parent = (*self)->highest_child;                           \
    }                                                                     \
    else if (!here->lower_child)                                          \
    {                                                                     \
      here->lower_child = newNode;                                        \
      newNode->parent = here;                                             \
    }                                                                     \
    else                                                                  \
    {                                                                     \
      here->lower_child->highest_child->higher_child = newNode;           \
      newNode->parent = here->lower_child->highest_child;                 \
    }                                                                     \
                                                                          \
    /* Retrace the tree up from the modified node to the root */          \
    BNODE_RETRACE_##K##_##T (newNode->parent, 0);                         \
                                                                          \
    /* Self-balancing */                                                  \
    BNODE_##K##_##T *new_root = BNODE_BALANCE_##K##_##T (newNode->parent);\
    if (new_root)                                                         \
      *self = new_root;                                                   \
                                                                          \
    return EXIT_SUCCESS;                                                  \
  }                                                                       \
\
  static size_t BNODE_TREE_ADD_##K##_##T(BNODE_##K##_##T **self,          \
                                      BNODE_##K##_##T *newNode,           \
                                      int (*lt)(K, K))                    \
  {                                                                       \
    if (newNode->parent || newNode == *self)                              \
    {                                                                     \
      errno = EINVAL;                                                     \
      return 0;                                                           \
    }                                                                     \
                                                                          \
    BNODE_##K##_##T *lower = newNode->lower_child;                        \
    BNODE_##K##_##T *higher = newNode->higher_child;                      \
                                                                          \
    /* newNode isolation */                                               \
    newNode->lower_child = newNode->higher_child = 0;                     \
    newNode->lowest_child = newNode->highest_child = newNode;             \
    newNode->size = newNode->depth = 1;                                   \
                                                                          \
    /* Add newNode in the tree */                                         \
    size_t ret = 0;                                                       \
    /* Go to leaf */                                                      \
    BNODE_##K##_##T *leaf = *self;                                        \
    while (1)                                                             \
    {                                                                     \
      int comp = BNODE_CMP_KEY_##K##_##T (&newNode, &leaf, &lt);          \
      if (comp > 0)                                                       \
      {                                                                   \
        if (leaf->higher_child)                                           \
        {                                                                 \
          leaf = leaf->higher_child;                                      \
          continue;                                                       \
        }                                                                 \
        else                                                              \
        {                                                                 \
          leaf->higher_child = newNode;                                   \
          newNode->parent = leaf;                                         \
          ret = newNode->size;                                            \
          break;                                                          \
        }                                                                 \
      }                                                                   \
      else if (comp == 0 && (*self)->unique)                              \
      {                                                                   \
        BNODE_DESTROY_##K##_##T (newNode); /* not inserted => destroyed */\
        ret = 0;                                                          \
        break;                                                            \
      }                                                                   \
      else /* comp <= 0 && (comp != 0 || !self->unique)                */ \
      {                                                                   \
        if (leaf->lower_child)                                            \
        {                                                                 \
          leaf = leaf->lower_child;                                       \
          continue;                                                       \
        }                                                                 \
        else                                                              \
        {                                                                 \
          leaf->lower_child = newNode;                                    \
          newNode->parent = leaf;                                         \
          ret = newNode->size;                                            \
          break;                                                          \
        }                                                                 \
      }                                                                   \
    }                                                                     \
                                                                          \
    if (ret)                                                              \
    {                                                                     \
      /* Retrace the tree up from the modified node to the root */        \
      BNODE_RETRACE_##K##_##T (newNode->parent, 0);                       \
                                                                          \
      /* Self-balancing */                                                \
      BNODE_##K##_##T *new_root = BNODE_BALANCE_##K##_##T (newNode->parent);\
      if (new_root)                                                       \
        *self = new_root;                                                 \
    }                                                                     \
                                                                          \
    /* Add leaves of newNode in the tree */                               \
    /* (allow to add a tree in a tree) */                                 \
    if (lower)                                                            \
    {                                                                     \
      lower->parent = 0;                                                  \
      ret += BNODE_TREE_ADD_##K##_##T(self, lower, lt);                   \
    }                                                                     \
    if (higher)                                                           \
    {                                                                     \
      higher->parent = 0;                                                 \
      ret += BNODE_TREE_ADD_##K##_##T(self, higher, lt);                  \
    }                                                                     \
                                                                          \
    return ret;                                                           \
  }                                                                       \
\
  static BNODE_##K##_##T *BNODE_REMOVE_##K##_##T( BNODE_##K##_##T *node ) \
  {                                                                       \
    BNODE_##K##_##T *new_root = 0;                                        \
    BNODE_##K##_##T *invalidated = 0;                                     \
                                                                          \
    if (node->lower_child && node->higher_child)                          \
    {                                                                     \
      /* Makes use of the method proposed by T. Hibbard in 1962 that   */ \
      /* changes the heights of the subtrees of 'node' by at most one. */ \
      /* Extra constraint imposed by from the interface:               */ \
      /* The pointer 'node' passed as an argument must be destroyable  */ \
      /* by the caller. Thus it cannot be reused as a placeholder for  */ \
      /* the key and data of its predecessor or successor. Therefore,  */ \
      /* the predecessor or successor will rather be moved in place of */ \
      /* node.                                                         */ \
                                                                          \
      BNODE_##K##_##T *hibbard = 0; /* predecessor or successor */        \
      BNODE_##K##_##T *child = 0;   /* 'hibbard' 's child       */        \
                                                                          \
      /* Retrieve and remove 'hibbard' from the tree.               */    \
      /* - Retrieve hibbard and its child.                          */    \
      /*   'hibbard' can have omly at most one child because        */    \
      /*   its successor or predecessor is the upper node 'node'.   */    \
      if (node->lower_child->depth > node->higher_child->depth)           \
      {                                                                   \
        hibbard = node->lower_child->highest_child; /* predecessor */     \
        /* N.B.: hibbard can sometimes be equal to node->lower_child */   \
        assert (hibbard->higher_child == 0);                              \
        child = hibbard->lower_child;                                     \
      }                                                                   \
      else                                                                \
      {                                                                   \
        hibbard = node->higher_child->lowest_child; /* successor */       \
        /* N.B.: hibbard can sometimes be equal to node->higher_child */  \
        assert (hibbard->lower_child == 0);                               \
        child = hibbard->higher_child;                                    \
      }                                                                   \
                                                                          \
      /* - Remove hibbard from the tree: change the 2 links           */  \
      /*   (from parent and child) pointing to hibbard.               */  \
      assert (hibbard->parent != 0);                                      \
      /* N.B.: node's children are changed here if hibbard is a node's child */ \
      if (hibbard->parent->lower_child == hibbard)                        \
        hibbard->parent->lower_child = child;                             \
      else if (hibbard->parent->higher_child == hibbard)                  \
        hibbard->parent->higher_child = child;                            \
      if (child)                                                          \
        child->parent = hibbard->parent;                                  \
                                                                          \
      /* Invalidate the modified node */                                  \
      invalidated = hibbard->parent;                                      \
                                                                          \
      /* Move 'hibbard' to the place of 'node' in the tree.    */         \
      /* - Changes the 6 links of hibbard for the ones of node */         \
      hibbard->parent = node->parent;                                     \
      hibbard->lower_child = node->lower_child;                           \
      hibbard->higher_child = node->higher_child;                         \
      /* N.B.: node's children could have changed here if hibbard was a node's child */ \
      if (node->lower_child)                                              \
        node->lower_child->parent = hibbard;                              \
      if (node->higher_child)                                             \
        node->higher_child->parent = hibbard;                             \
      if (node->parent)                                                   \
      {                                                                   \
        if (node->parent->lower_child == node)                            \
          node->parent->lower_child = hibbard;                            \
        else if (node->parent->higher_child == node)                      \
          node->parent->higher_child = hibbard;                           \
      }                                                                   \
                                                                          \
      /* Invalidate the modified node */                                  \
      if (invalidated == node)                                            \
        invalidated = hibbard;                                            \
                                                                          \
      /* Set the new root of the tree */                                  \
      new_root = hibbard;                                                 \
    }                                                                     \
    else /* if (!node->lower_child || !node->higher_child) */             \
    {                                                                     \
      /* Set the new root of the tree */                                  \
      if (node->lower_child)                                              \
        new_root = node->lower_child;                                     \
      else if (node->higher_child)                                        \
        new_root = node->higher_child;                                    \
      else                                                                \
        new_root = 0;                                                     \
                                                                          \
      /* Update links to nodes's parent and remove 'node' from the tree */\
      if (node->parent)                                                   \
      {                                                                   \
        if (node->parent->lower_child == node)                            \
          node->parent->lower_child = new_root;                           \
        else if (node->parent->higher_child == node)                      \
          node->parent->higher_child = new_root;                          \
      }                                                                   \
      if (new_root)                                                       \
        new_root->parent = node->parent;                                  \
                                                                          \
      /* Invalidate the modified node */                                  \
      invalidated = node->parent;                                         \
    }                                                                     \
                                                                          \
    /* Retrace the tree up from the invalidated node to the root */       \
    BNODE_RETRACE_##K##_##T (invalidated, 0);                             \
                                                                          \
    /* Isolate 'node' to make it destroyable or movable */                \
    node->parent = 0;                                                     \
    node->lower_child = node->higher_child = 0;                           \
    node->lowest_child = node->highest_child = node;                      \
    node->depth = node->size = 1;                                         \
                                                                          \
    return new_root;                                                      \
  }                                                                       \
\
  static BNODE_##K##_##T *BNODE_INDEX_##K##_##T (BNODE_##K##_##T *self, size_t index) \
  {                                                                               \
    if (index >= self->size)                                                      \
    {                                                                             \
      fprintf (stderr, "ERROR: " "Index %zu out of range [0, %zu].\n"             \
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
  struct __useless_struct_BNODE_IMPL_##K##_##T

#endif
