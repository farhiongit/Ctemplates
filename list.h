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

#ifndef __LIST_H__
#define __LIST_H__

#include "bnode.h"
#include "vfunc.h"
#include <stdio.h>
#include <signal.h>

typedef int __list_dummy__ ;

//
// DECLARE_LIST
// Purpose: Declares a linked list of a specific type on the heap.
//
#define DECLARE_LIST( TYPE )     \
\
  DECLARE_BNODE(__list_dummy__, TYPE);            \
\
  struct _LIST_##TYPE;           \
\
  typedef struct _LIST_VTABLE_##TYPE                                                                 \
  {                                                                                                  \
    BNODE___list_dummy___##TYPE *(*CreateNode)( __list_dummy__ key, int unique );                    \
    void (*Clear) ( struct _LIST_##TYPE *self );                                                     \
    BNODE___list_dummy___##TYPE * (*Insert) ( struct _LIST_##TYPE *self, BNODE___list_dummy___##TYPE *here, BNODE___list_dummy___##TYPE *node );  \
    int (*Remove) ( struct _LIST_##TYPE *self, BNODE___list_dummy___##TYPE *node );                                \
    size_t (*Unique) ( struct _LIST_##TYPE *self, int (*less_than) (TYPE, TYPE));                    \
    void (*Sort) ( struct _LIST_##TYPE *self, int (*less_than) (TYPE, TYPE));                        \
    int (*Move) ( struct _LIST_##TYPE *to, BNODE___list_dummy___##TYPE *hereto, struct _LIST_##TYPE *from, BNODE___list_dummy___##TYPE *herefrom );     \
    int (*Swap) ( struct _LIST_##TYPE *la, BNODE___list_dummy___##TYPE *nodea, struct _LIST_##TYPE *lb, BNODE___list_dummy___##TYPE *nodeb );     \
    int (*Reverse) ( struct _LIST_##TYPE *self, BNODE___list_dummy___##TYPE *nodea, BNODE___list_dummy___##TYPE *nodeb );     \
    BNODE___list_dummy___##TYPE * (*End) ( struct _LIST_##TYPE *self );                              \
  } _LIST_VTABLE_##TYPE;                                                                             \
\
  typedef struct _LIST_##TYPE          \
  {                                    \
    BNODE___list_dummy___##TYPE *null; \
    BNODE___list_dummy___##TYPE *root; \
    const _LIST_VTABLE_##TYPE *vtable; \
    int tree_locked;                   \
  } LIST_##TYPE;                       \
\
  LIST_##TYPE *LIST_CREATE_##TYPE( void );                                  \
  struct __useless_struct_LIST_##TYPE

/// Creates and returns a new list object of specified type.
/// @param [in] TYPE typename of elements hold in the list.
#define LIST_CREATE( TYPE ) \
  LIST_CREATE_##TYPE()

/// Inserts a node of specified type in a list.
/// @param [in] listSelf pointer to list
/// @param [in] here Pointer to the node before which a new node is inserted.
/// @param [in] data Value of type TYPE to be inserted in the list.
/// @returns Pointer to the inserted node, or 0 in case of failure (errno set to ENOMEM).
#define LIST_INSERT(listSelf, here, data) \
  ((listSelf)->vtable->Insert ((listSelf), (here), \
                               (listSelf)->null->vtable->Assign((listSelf)->vtable->CreateNode (0, 0), data)))

#define LIST_APPEND(listSelf, data) \
  LIST_INSERT (listSelf, LIST_END (listSelf), data)

/// Removes (deallocates) a node from a list.
/// @param [in] listSelf Pointer to list
/// @param [in] node Pointer to the node to remove from the list.
#define LIST_REMOVE( listSelf, node ) \
  ((listSelf)->vtable->Remove ((listSelf), (node)))

/// Removes (deallocates) a list and all its nodes.
/// @param [in] listSelf Pointer to list
#define LIST_DESTROY( listSelf ) \
  do { (listSelf)->vtable->Clear ( (listSelf) ); free ((listSelf)); } while(0)

/// Removes (deallocates) all the nodes of a list.
/// @param [in] listSelf Pointer to list
#define LIST_CLEAR( listSelf ) \
  do { (listSelf)->vtable->Clear ( (listSelf) ); } while(0)

///Moves an element from one list to another.
/// @param [in] to pointer to the destination list
/// @param [in] hereto Pointer to the node where to move the \p herefrom node
/// @param [in] from pointer to the source list
/// @param [in] herefrom Pointer to the node to move into the list \p to
#define LIST_MOVE( to, hereto, from, herefrom ) \
  ((to)->vtable->Move ( (to), (hereto), (from), (herefrom) ))

/// Gets the first node in a list.
/// @param [in] listSelf pointer to list
/// @returns the first node in the list.
#define LIST_BEGIN( listSelf ) \
  ((listSelf)->root ? BNODE_FIRST ((listSelf)->root) : LIST_END (listSelf))

/// Gets the last node in a list.
/// @param [in] listSelf pointer to list
/// @returns the last node in the list.
#define LIST_LAST( listSelf ) \
  ((listSelf)->root ? BNODE_LAST ((listSelf)->root) : LIST_END (listSelf))

/// Gets the one-past-last node in a list.
/// @param [in] listSelf pointer to list
/// @returns the one-past-last node in the list.
#define LIST_END( listSelf ) \
  ((listSelf)->vtable->End ((listSelf)))

/// Gets the number of elements in a list.
/// @param [in] listSelf pointer to list.
/// @returns the number of elements in the list.
#define LIST_SIZE( listSelf ) \
  ((listSelf)->root ? BNODE_SIZE ((listSelf)->root) : (size_t)0)

/// Indicates a list is empty.
/// @param [in] listSelf pointer to list.
/// @returns 1 if the list is empty, 0 otherwise.
#define LIST_IS_EMPTY( listSelf ) \
  (LIST_SIZE ((listSelf)) == 0)

#define LIST_UNIQUE2( listSelf, less_than ) \
  ((listSelf)->vtable->Unique ( (listSelf), (less_than) ))

#define LIST_UNIQUE1( listSelf ) LIST_UNIQUE2( listSelf, 0 )

/// Removes redundant consecutive elements from a list.
/// @param [in] listSelf pointer to list.
/// @param [in, optional] less than operator. By default, the less than operator associated to the list, or to the type is used.
/// @returns The number of removed redundant elements.
#define LIST_UNIQUE(...) VFUNC(LIST_UNIQUE, __VA_ARGS__)

#define LIST_SORT2( listSelf, less_than ) \
  do { (listSelf)->vtable->Sort ( (listSelf), (less_than) ) ; } while(0)

#define LIST_SORT1( listSelf ) LIST_SORT2( listSelf, 0 )

/// Sorts elements in a list.
/// @param [in] listSelf pointer to list.
/// @param [in, optional] less than operator. By default, the less than operator associated to the list, or to the type is used.
#define LIST_SORT(...) VFUNC(LIST_SORT, __VA_ARGS__)

/// Defines a less than operator associated to a list.
/// @param [in] listSelf pointer to list.
/// @param [in] less_than_operator less than operator with signature `int (*)(TYPE, TYPE)`.
#define LIST_SET_LESS_THAN_OPERATOR( listSelf, less_than_operator ) \
  do { (listSelf)->LessThanValue = (less_than_operator) ; } while(0)

/// Typename of the template list.
/// @param [in] TYPE typename of elements hold in the list.
#define LIST( TYPE ) \
  LIST_##TYPE

#define LIST_FIND4( listSelf, begin, value, neq ) \
  ((listSelf)->root ? BNODE_FIND_VALUE3((begin), (value), (neq)): LIST_END(listSelf))

#define LIST_FIND3( listSelf, value, neq ) LIST_FIND4( listSelf, BNODE_FIRST((listSelf)->root), value, neq )

#define LIST_FIND2( listSelf, value ) LIST_FIND3( listSelf, value, 0 )

#define LIST_FIND(...) VFUNC(LIST_FIND, __VA_ARGS__)

#define LIST_TRAVERSE4( map, callback, param, stop_condition ) \
  do { if ((map)->root) { \
         (map)->tree_locked = 1 ; \
         BNODE_TRAVERSE4((map)->root, (callback), (param), (stop_condition)) ; \
         (map)->tree_locked = 0 ; \
       } \
     } while (0)

#define LIST_TRAVERSE3( map, callback, param ) LIST_TRAVERSE4( map, callback, param, EXIT_FAILURE )

#define LIST_TRAVERSE2( map, callback ) LIST_TRAVERSE3( map, callback, map )

#define LIST_TRAVERSE(...) VFUNC(LIST_TRAVERSE, __VA_ARGS__)

#define LIST_FOR_EACH6( map, begin, end, callback, param, stop_condition ) \
  ((map)->root ? (begin)->vtable->ForEach((begin), (end), (callback), (param), (stop_condition), BNODE_FORWARD) : LIST_END (map))

#define LIST_FOR_EACH5( map, begin, callback, param, stop_condition ) LIST_FOR_EACH6( map, begin, LIST_END (map), callback, param, stop_condition )

#define LIST_FOR_EACH4( map, callback, param, stop_condition ) LIST_FOR_EACH5( map, SET_BEGIN( map ), callback, param, stop_condition )

#define LIST_FOR_EACH3( map, callback, param ) LIST_FOR_EACH4( map, callback, param, EXIT_FAILURE )

#define LIST_FOR_EACH2( map, callback ) LIST_FOR_EACH3( map, callback, map )

#define LIST_FOR_EACH(...) VFUNC(LIST_FOR_EACH, __VA_ARGS__)

/// Reverse the order of the nodes in the list.
/// @param [in] listSelf Pointer to a list.
#define LIST_REVERSE1( listSelf ) \
  LIST_REVERSE3 (listSelf, LIST_BEGIN (listSelf), LIST_END (listSelf))

/// Rotates the nodes of the list to the left.
/// @param [in] listSelf Pointer to a list.
#define LIST_ROTATE_LEFT( listSelf) \
  do { if (LIST_SIZE(listSelf) > 1) LIST_MOVE(listSelf, LIST_END(listSelf), listSelf, LIST_BEGIN(listSelf)); } while(0)

/// Rotates the nodes of the list to the right.
/// @param [in] listSelf Pointer to a list.
#define LIST_ROTATE_RIGHT( listSelf) \
  do { if (LIST_SIZE(listSelf) > 1) LIST_MOVE(listSelf, LIST_BEGIN(listSelf), listSelf, LIST_LAST(listSelf)); } while(0)

#define LIST_INDEX( map, index) \
  ((map)->root ? BNODE_INDEX((map)->root, (index)) : (fprintf (stderr, "ERROR: " "List is empty.\nABORT\n"), fflush (0), raise (SIGABRT), LIST_END(map)))

#define LIST_SWAP( la, nodea, lb, nodeb) \
  ((la)->vtable->Swap ((la), (nodea), (lb), (nodeb)))

#define LIST_REVERSE3( la, nodea, nodeb) \
  ((la)->vtable->Reverse ((la), (nodea), (nodeb)))

#define LIST_REVERSE(...) VFUNC(LIST_REVERSE, __VA_ARGS__)

#define LNODE( K ) \
  BNODE___list_dummy___##K

#define LNODE_PREVIOUS(n) BNODE_PREVIOUS(n)
#define LNODE_NEXT(n) BNODE_NEXT(n)
#define LNODE_COPY(n) BNODE_COPY(n)
#define LNODE_ASSIGN(n,v) BNODE_ASSIGN(n,v)
#define LNODE_VALUE(n) BNODE_VALUE(n)
#define LNODE_FOR_EACH(...) BNODE_FOR_EACH(__VA_ARGS__)

#endif
