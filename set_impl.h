////////////////////////////////////////////////////
// Original Author: Laurent Farhi
// Date:   22/1/2017
// Contact: lfarhi@sfr.fr
//
// Based on design from Randy Gaul's Game Programming Blog http://cecilsunkure.blogspot.fr/2012/08/generic-programming-in-c.html
////////////////////////////////////////////////////

#pragma once

#ifndef __SET_IMPL_H__
#define __SET_IMPL_H__

#define _GNU_SOURCE
#include <stdlib.h>
#include <errno.h>
#include "bnode_impl.h"
#include "set.h"
#include "defops.h"

DEFINE_OPERATORS(__set_dummy__)

//
// DEFINE_SET
// Purpose: Defines a linked list of a specific type on the heap.
//
#define DEFINE_SET( K )                              \
\
  DEFINE_BNODE( K, __set_dummy__ )                       \
\
  static void SET_CLEAR_##K ( SET_##K *self );                                                 \
  static void SET_DESTROY_##K ( SET_##K *self );                                               \
  static BNODE_##K##___set_dummy__ *SET_INSERT_##K ( SET_##K *self, BNODE_##K##___set_dummy__ *node ); \
  static int SET_REMOVE_##K ( SET_##K *self, BNODE_##K##___set_dummy__ *node );                \
  static int SET_MOVE_##K ( SET_##K *to, SET_##K *from, BNODE_##K##___set_dummy__ *herefrom ); \
  static BNODE_##K##___set_dummy__ *SET_END_##K ( SET_##K *self );                             \
\
  static const _SET_VTABLE_##K SET_VTABLE_##K =      \
  {                                                  \
    BNODE_CREATE_##K##___set_dummy__,                \
    SET_CLEAR_##K,                                   \
    SET_DESTROY_##K,                                 \
    SET_INSERT_##K,                                  \
    SET_REMOVE_##K,                                  \
    SET_MOVE_##K,                                    \
    SET_END_##K,                                     \
  };                                                 \
\
  SET_##K *SET_CREATE_##K( int (*less_than_operator) (K, K), int unique )    \
  {                                                                          \
    SET_##K *linkedList = malloc( sizeof( *linkedList ) );                   \
    if (!linkedList)                                                         \
      return 0;                                                              \
                                                                             \
    linkedList->vtable = &SET_VTABLE_##K;                                    \
    linkedList->LessThan = less_than_operator;                               \
    linkedList->unique = unique;                                             \
    linkedList->tree_locked = 0;                                             \
                                                                             \
    linkedList->root = 0;                                                    \
                                                                             \
    return linkedList;                                                       \
  }                                                                          \
\
  static BNODE_##K##___set_dummy__ *SET_INSERT_##K ( SET_##K *self, BNODE_##K##___set_dummy__ *node )  \
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
    K key = COPY_##K ? COPY_##K(*BNODE_KEY (node)) : *BNODE_KEY (node);    \
                                                                           \
    BNODE_##K##___set_dummy__ *ret =                                       \
           !self->root ?                                                   \
           self->root = node :                                             \
           BNODE_TREE_ADD(self->root, node, self->LessThan) ?              \
           node :                                                          \
           BNODE_FIND_KEY (BNODE_FIRST (self->root), key, self->LessThan); \
                                                                           \
    if (DESTROY_##K) DESTROY_##K (key);                                    \
    return ret;                                                            \
  }                                                                        \
\
  static int SET_REMOVE_##K ( SET_##K *self, BNODE_##K##___set_dummy__ *node )   \
  {                                                                        \
    /* Go to root */                                                       \
    BNODE_##K##___set_dummy__ *n;                                          \
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
    BNODE_##K##___set_dummy__ *new_node = BNODE_REMOVE_##K##___set_dummy__(node);       \
    if (self->root == node)                                          \
      self->root = new_node;                                         \
    BNODE_DESTROY_##K##___set_dummy__(node);                         \
    return EXIT_SUCCESS;                                             \
  }                                                                  \
\
  static int SET_MOVE_##K ( SET_##K *to, SET_##K *from, BNODE_##K##___set_dummy__ *herefrom )  \
  {                                                                        \
    if (to == from)                                                        \
      return EXIT_SUCCESS;                                                 \
                                                                           \
    /* Go to root */                                                       \
    BNODE_##K##___set_dummy__ *n;                                          \
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
    BNODE_##K##___set_dummy__ *new_node = BNODE_REMOVE_##K##___set_dummy__(herefrom);       \
    if (from->root == herefrom)                                            \
      from->root = new_node;                                               \
    SET_INSERT_##K (to, herefrom);                                         \
                                                                           \
    return EXIT_SUCCESS;                                                   \
  }                                                                        \
\
  static void SET_CLEAR_##K ( SET_##K *self )  \
  {                                                   \
    if (self->root)                                   \
      BNODE_DESTROY_##K##___set_dummy__(self->root);  \
    self->root = 0;                                   \
  }                                                   \
\
  static void SET_DESTROY_##K ( SET_##K *self )   \
  {                                               \
    SET_CLEAR_##K (self);                         \
    free( self );                                 \
  }                                               \
\
  static BNODE_##K##___set_dummy__ *SET_END_##K ( SET_##K *self ) \
  {                                                             \
    (void)self;                                                 \
    return 0;                                                   \
  }                                                             \

#endif
