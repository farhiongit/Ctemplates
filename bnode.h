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

#ifndef __BNODE_H__
#define __BNODE_H__

#include <stdlib.h>
#include "vfunc.h"

typedef enum {
  BNODE_FORWARD,
  BNODE_BACKWARD,
} BNODE_DIRECTION;

#define DECLARE_BNODE( K, T )     \
\
  struct _BNODE_##K##_##T;           \
\
  typedef struct _BNODE_VTABLE_##K##_##T                                                             \
  {                                                                                                  \
    struct _BNODE_##K##_##T *(*Copy)  ( struct _BNODE_##K##_##T *self );                             \
    void (*Destroy)  ( struct _BNODE_##K##_##T *self );                                              \
    struct _BNODE_##K##_##T *(*Remove)  ( struct _BNODE_##K##_##T *self );                           \
    int (*LessThanKey)  ( K a, K b, int (*lt)(K, K) );                                               \
    int (*CmpKey)  (const void *a, const void *b, void* arg);                                        \
    int (*LessThanValue)  ( T a, T b, int (*lt)(T, T) );                                             \
    int (*CmpValue)  (const void *a, const void *b, void* arg);                                      \
    struct _BNODE_##K##_##T *(*ForEach)  ( struct _BNODE_##K##_##T *begin,                           \
                                       struct _BNODE_##K##_##T *end,                                 \
                                       int (*callback)( struct _BNODE_##K##_##T *, void * ),         \
                                       void *param, int stop_condition, BNODE_DIRECTION direction ); \
    struct _BNODE_##K##_##T * (*Assign)  ( struct _BNODE_##K##_##T *self, T value );                 \
    struct _BNODE_##K##_##T *(*FindKey)  ( struct _BNODE_##K##_##T *begin,                           \
                                      struct _BNODE_##K##_##T *end,                                  \
                                      K key, int (*lt)(K, K), BNODE_DIRECTION direction );           \
    struct _BNODE_##K##_##T *(*FindValue)  ( struct _BNODE_##K##_##T *begin,                         \
                                      struct _BNODE_##K##_##T *end,                                  \
                                      T value, int (*lt)(T, T), BNODE_DIRECTION direction );         \
    struct _BNODE_##K##_##T *(*Previous) ( struct _BNODE_##K##_##T *n );                             \
    struct _BNODE_##K##_##T *(*Next) ( struct _BNODE_##K##_##T *n );                                 \
    int (*Add) ( struct _BNODE_##K##_##T **self, struct _BNODE_##K##_##T *node, int (*lt)(K, K) );   \
    int (*Insert) ( struct _BNODE_##K##_##T **self, struct _BNODE_##K##_##T *here, struct _BNODE_##K##_##T *newNode); \
    struct _BNODE_##K##_##T *(*Index) ( struct _BNODE_##K##_##T *n, size_t index);                   \
    struct _BNODE_##K##_##T *(*Key) ( struct _BNODE_##K##_##T *n, K key, int (*lt)(K, K));           \
    int (*Traverse) ( struct _BNODE_##K##_##T *tree,                                                 \
                      int (*callback)( struct _BNODE_##K##_##T *, void * ), void *param,             \
                      int stop_condition );                                                          \
  } _BNODE_VTABLE_##K##_##T;                                                                         \
\
  typedef struct _BNODE_##K##_##T          \
  {                                        \
    K key;                                 \
    T data;                                \
    struct _BNODE_##K##_##T *parent;       \
    struct _BNODE_##K##_##T *higher_child; \
    struct _BNODE_##K##_##T *lower_child;  \
    struct _BNODE_##K##_##T *highest_child;\
    struct _BNODE_##K##_##T *lowest_child; \
    size_t size;                           \
    size_t depth;                          \
    int unique;                            \
    const _BNODE_VTABLE_##K##_##T *vtable; \
  } BNODE_##K##_##T;                       \
\
  BNODE_##K##_##T *BNODE_CREATE_##K##_##T( K key, int unique );                               \
  
#define BNODE_CREATE( K, T ) \
  BNODE_CREATE_##K##_##T

/// Gets key from node.
/// @param [in] node Pointer to the node which value is to be returned.
/// @returns key hold by the node.
#define BNODE_KEY(node) \
  (&((node)->key))

/// Gets data from node.
/// @param [in] node Pointer to the node which value is to be returned.
/// @returns Value hold by the node.
#define BNODE_VALUE(node) \
  (&((node)->data))

/// Typename of the template nodes for maps of type (K, T).
/// @param [in] K Key typename.
/// @param [in] T Value typename.
#define BNODE( K, T ) \
  BNODE_##K##_##T

#define BNODE_DESTROY( self ) \
  do { (self)->vtable->Destroy ((self)) ; } while (0)

#define BNODE_REMOVE( self ) \
  ((self)->vtable->Remove ((self)))

#define BNODE_COPY( self ) \
  ((self)->vtable->Copy ((self)))

/// Assigns a new value to a node
/// @param [in] self Pointer to the node which value is to be modified.
/// @param [in] value Value to assign to the node.
#define BNODE_ASSIGN( self, value ) \
  do { (self)->vtable->Assign ((self), (value)) ; } while (0)

/// Yields the previous node.
/// @param [in] self Pointer to a node.
/// @return Previous node.
#define BNODE_PREVIOUS( self ) \
  ((self)->vtable->Previous ((self)))

#define BNODE_FIRST( self ) \
  ((self)->lowest_child)

/// Yields the next node.
/// @param [in] self Pointer to a node.
/// @return Next node.
#define BNODE_NEXT( self ) \
  ((self)->vtable->Next ((self)))

#define BNODE_LAST( self ) \
  ((self)->highest_child)

#define BNODE_SIZE( self ) \
  ((self)->size)

#define BNODE_LESS_THAN_KEY( a, b ) \
  ((a)->vtable->LessThanKey((a)->key, (b)->key, 0))

#define BNODE_CMP_KEY( a, b ) \
  ((a)->vtable->CmpKey(&(a), &(b), 0))

#define BNODE_LESS_THAN_VALUE( a, b ) \
  ((a)->vtable->LessThanValue((a)->data, (b)->data, 0))

#define BNODE_CMP_VALUE( a, b ) \
  ((a)->vtable->CmpValue(&(a), &(b), 0))

#define BNODE_FOR_EACH5(begin, end, function, param, stop_condition) \
  ((begin)->vtable->ForEach((begin), (end), (function), (param), (stop_condition), BNODE_FORWARD))

#define BNODE_FOR_EACH4(begin, end, function, param) BNODE_FOR_EACH5(begin, end, function, param, EXIT_FAILURE)

#define BNODE_FOR_EACH3(begin, end, function) BNODE_FOR_EACH4(begin, end, function, 0)

/// Loops forward on nodes and apply a function on each node as long as the function returns RETURN_SUCCESS.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in] end Pointer to the ending node of the loop.
/// @param [in] function Function to be applied to each node. This function has signature RETURN_TYPE (*function)(BNODE(K, T) *, void *).
/// @param [in, optional] arg This argument is passed to the second argument of each invocation of \p function. Defaults to 0.
/// @returns Pointer to the last node processed by the loop.
#define BNODE_FOR_EACH(...) VFUNC(BNODE_FOR_EACH, __VA_ARGS__)

#define BNODE_FOR_EACH_REVERSE4(begin, end, function, param) \
  ((begin)->vtable->ForEach((begin), (end), (function), (param), EXIT_FAILURE, BNODE_BACKWARD))

#define BNODE_FOR_EACH_REVERSE3(begin, end, function) BNODE_FOR_EACH_REVERSE4(begin, end, function, 0)

/// Loops backward on nodes and apply a function on each node as long as the function returns RETURN_SUCCESS.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in] end Pointer to the ending node of the loop.
/// @param [in] function Function to be applied to each node. This function has signature RETURN_TYPE (*function)(BNODE(K, T) *, void *).
/// @param [in, optional] arg This argument is passed to the second argument of each invocation of \p function. Defaults to 0.
/// @returns Pointer to the last node processed by the loop.
#define BNODE_FOR_EACH_REVERSE(...) VFUNC(BNODE_FOR_EACH_REVERSE, __VA_ARGS__)

#define BNODE_FIND4(begin, end, predicate, param) \
  ((begin)->vtable->ForEach((begin), (end), (predicate), (param), EXIT_SUCCESS, BNODE_FORWARD))

#define BNODE_FIND3(begin, predicate, param) BNODE_FIND4(begin, 0, predicate, param)

/// Loops forward on nodes and apply a function on each node as long as the predicate returns RETURN_FAILURE.
/// Useful to find the first element that verify a predicate with a specified param.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in, optional] end Pointer to the ending node of the loop. Defaults to the end of the collection.
/// @param [in] predicate Function to be applied to each node. This function has signature RETURN_TYPE (*predicate)(BNODE(K, T) *, void *).
/// @param [in] arg This argument is passed to the second argument of each invocation of \p function. Defaults to 0.
/// @returns Pointer to the last node processed by the loop.
#define BNODE_FIND(...) VFUNC(BNODE_FIND, __VA_ARGS__)

#define BNODE_FIND_REVERSE4(begin, end, predicate, param) \
  ((begin)->vtable->ForEach((begin), (end), (predicate), (param), EXIT_SUCCESS, BNODE_BACKWARD))

#define BNODE_FIND_REVERSE3(begin, predicate, param) BNODE_FIND_REVERSE4(begin, 0, predicate, param)

/// Loops backward on nodes and apply a function on each node as long as the predicate returns RETURN_FAILURE.
/// Useful to find the first element that verify a predicate with a specified param.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in, optional] end Pointer to the ending node of the loop. Defaults to the end of the collection.
/// @param [in] predicate Function to be applied to each node. This function has signature RETURN_TYPE (*predicate)(BNODE(K, T) *, void *).
/// @param [in] arg This argument is passed to the second argument of each invocation of \p function. Defaults to 0.
/// @returns Pointer to the last node processed by the loop.
#define BNODE_FIND_REVERSE(...) VFUNC(BNODE_FIND_REVERSE, __VA_ARGS__)

#define BNODE_FIND_KEY4(begin, end, key, lt) \
  ((begin)->vtable->FindKey((begin), (end), (key), (lt), BNODE_FORWARD))

#define BNODE_FIND_KEY3(begin, key, lt) BNODE_FIND_KEY4(begin, 0, key, lt)

#define BNODE_FIND_KEY2(begin, key) BNODE_FIND_KEY3(begin, key, 0)

/// Finds forward the first node which matches a given key.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in, optional] end Pointer to the ending node of the loop. Defaults to the end of the collection.
/// @param [in] value Value to be searched for.
/// @returns A pointer to the first node between \p begin and \p end which value equals to \p value. Or 0 if no such node exists.
#define BNODE_FIND_KEY(...) VFUNC(BNODE_FIND_KEY, __VA_ARGS__)

#define BNODE_GET_KEY(tree, key, lt) \
  ((tree)->vtable->Key((tree), (key), (lt)))

#define BNODE_FIND_KEY_REVERSE4(begin, end, key, lt) \
  ((begin)->vtable->FindKey((begin), (end), (key), (lt), BNODE_BACKWARD))

#define BNODE_FIND_KEY_REVERSE3(begin, key, lt) BNODE_FIND_KEY_REVERSE4(begin, 0, key, lt)

#define BNODE_FIND_KEY_REVERSE2(begin, key) BNODE_FIND_KEY_REVERSE3(begin, key, 0)

/// Finds backward the first node which matches a given key.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in, optional] end Pointer to the ending node of the loop. Defaults to the end of the collection.
/// @param [in] value Value to be searched for.
/// @returns A pointer to the first node between \p begin and \p end which value equals to \p value. Or 0 if no such node exists.
#define BNODE_FIND_KEY_REVERSE(...) VFUNC(BNODE_FIND_KEY_REVERSE, __VA_ARGS__)

#define BNODE_FIND_VALUE4(begin, end, value, lt) \
  ((begin)->vtable->FindValue((begin), (end), (value), (lt), BNODE_FORWARD))

#define BNODE_FIND_VALUE3(begin, value, lt) BNODE_FIND_VALUE4(begin, 0, value, lt)

#define BNODE_FIND_VALUE2(begin, value) BNODE_FIND_VALUE3(begin, value, 0)

/// Finds forward the first node which matches a given value.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in, optional] end Pointer to the ending node of the loop. Defaults to the end of the collection.
/// @param [in] value Value to be searched for.
/// @returns A pointer to the first node between \p begin and \p end which value equals to \p value. Or 0 if no such node exists.
#define BNODE_FIND_VALUE(...) VFUNC(BNODE_FIND_VALUE, __VA_ARGS__)

#define BNODE_FIND_VALUE_REVERSE4(begin, end, value, lt) \
  ((begin)->vtable->FindValue((begin), (end), (value), (lt), BNODE_BACKWARD))

#define BNODE_FIND_VALUE_REVERSE3(begin, value, lt) BNODE_FIND_VALUE_REVERSE4(begin, 0, value, lt)

#define BNODE_FIND_VALUE_REVERSE2(begin, value) BNODE_FIND_VALUE_REVERSE3(begin, value, 0)

/// Finds backward the first node which matches a given value.
/// @param [in] begin Pointer to the starting node of the loop.
/// @param [in, optional] end Pointer to the ending node of the loop. Defaults to the end of the collection.
/// @param [in] value Value to be searched for.
/// @returns A pointer to the first node between \p begin and \p end which value equals to \p value. Or 0 if no such node exists.
#define BNODE_FIND_VALUE_REVERSE(...) VFUNC(BNODE_FIND_VALUE_REVERSE, __VA_ARGS__)

#define BNODE_TREE_ADD(tree, n, lt) \
  ((tree)->vtable->Add(&(tree), (n), (lt)))

#define BNODE_TREE_INSERT_BEFORE(tree, here, new_node) \
  ((tree)->vtable->Insert(&(tree), (here), (new_node)))

#define BNODE_INDEX(tree, i) \
  ((tree)->vtable->Index((tree), (i)))

#define BNODE_TRAVERSE4(tree, callback, param, stop_condition) \
  ((tree)->vtable->Traverse((tree), (callback), (param), (stop_condition)))

#define BNODE_TRAVERSE5(begin, end, callback, param, stop_condition) \
  ((begin)->vtable->ForEach((begin), (end), (callback), (param), (stop_condition), BNODE_FORWARD))

#define BNODE_TRAVERSE(...) VFUNC(BNODE_TRAVERSE, __VA_ARGS__)

#define BNODE_CHANGE_TO_ROOT( n ) \
  do { while (n->parent) n = n->parent; } while (0)

#endif
