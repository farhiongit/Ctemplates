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

#ifndef __SET_H__
#define __SET_H__

#include "bnode.h"
#include "vfunc.h"

typedef int __set_dummy__ ;

//
// DECLARE_SET
// Purpose: Declares a linked list of a specific type on the heap.
//
#define DECLARE_SET( K )       \
\
  DECLARE_BNODE(K, __set_dummy__)  \
\
  struct _SET_##K;             \
\
  typedef struct _SET_VTABLE_##K                                                                        \
  {                                                                                                     \
    BNODE_##K##___set_dummy__ *(*CreateNode)( K key, int unique );                                      \
    void (*Clear) ( struct _SET_##K *self );                                                            \
    void (*Destroy) ( struct _SET_##K *self );                                                          \
    BNODE_##K##___set_dummy__ *(*Insert) ( struct _SET_##K *self, BNODE_##K##___set_dummy__ *node );    \
    int (*Remove) ( struct _SET_##K *self, BNODE_##K##___set_dummy__ *node );                           \
    int (*Move) ( struct _SET_##K *to, struct _SET_##K *from, BNODE_##K##___set_dummy__ *herefrom );    \
    BNODE_##K##___set_dummy__ *(*End) ( struct _SET_##K *self );                                        \
  } _SET_VTABLE_##K;                                                                                    \
\
  typedef struct _SET_##K            \
  {                                  \
    BNODE_##K##___set_dummy__ *root; \
    const _SET_VTABLE_##K *vtable;   \
    int (*LessThan) (K, K);          \
    int unique;                      \
    int tree_locked;                 \
  } SET_##K;                         \
\
  SET_##K *SET_CREATE_##K( int (*less_than_operator) (K, K), int unique );              \

#define SET_CREATE3( K, less_than_operator, unique ) \
  SET_CREATE_##K((less_than_operator), (unique))

#define SET_CREATE2( K, less_than_operator) SET_CREATE3( K, less_than_operator, 1 )

#define SET_CREATE1( K ) SET_CREATE2( K, 0 )

/// Creates and returns a new map of specified type.
/// @param [in] K typename of keys of elements hold in the map.
/// @param [in, optional] less_than_operator Function defining the less than operator int (*less_than_operator) (K, K).
/// @param [in, optional] unique If set to 1 (by default), elements are unique in the map, otherwise they can be duplicated.
#define SET_CREATE(...) VFUNC(SET_CREATE, __VA_ARGS__)

/// Inserts a node of specified type in the map.
/// @param [in] listSelf Pointer to collection.
/// @param [in] key Key of type K to be inserted in the map.
/// @returns 1 in case of success, or 0 in case of failure (errno set to ENOMEM).
#define SET_INSERT(listSelf, key) \
  ((listSelf)->vtable->Insert ((listSelf), (listSelf)->vtable->CreateNode ((key), (listSelf)->unique)))

/// Removes (deallocates) a node from a map.
/// @param [in] listSelf Pointer to map.
/// @param [in] node Pointer to the node to remove from the map.
#define SET_REMOVE( listSelf, node ) \
  ((listSelf)->vtable->Remove ((listSelf), (node)))

/// Removes (deallocates) a collection and all its nodes.
/// @param [in] listSelf Pointer to collection.
#define SET_DESTROY( listSelf ) \
  do { (listSelf)->vtable->Destroy ( (listSelf) ); } while(0)

/// Removes (deallocates) all the nodes from a collection.
/// @param [in] listSelf Pointer to collection.
#define SET_CLEAR( listSelf ) \
  do { (listSelf)->vtable->Clear ( (listSelf) ); } while(0)

/// Moves an element from one collection to another.
/// @param [in] to pointer to the destination collection
/// @param [in] from pointer to the source collection
/// @param [in] herefrom Pointer to the node to move into the collection \p to
#define SET_MOVE( to, from, herefrom ) \
  ((to)->vtable->Move ( (to), (from), (herefrom) ))

/// Gets the first node in a map.
/// @param [in] listSelf pointer to map.
/// @returns the first node in the map.
#define SET_BEGIN( listSelf ) \
  ((listSelf)->root ? BNODE_FIRST ((listSelf)->root) : SET_END (listSelf))

/// Gets the last node in a map.
/// @param [in] listSelf pointer to map.
/// @returns the last node in the map.
#define SET_LAST( listSelf ) \
  ((listSelf)->root ? BNODE_LAST ((listSelf)->root) : SET_END (listSelf))

/// Gets the one-past-last node in a map.
/// @param [in] listSelf pointer to map.
/// @returns the one-past-last node in the map.
#define SET_END( listSelf ) \
  ((listSelf)->vtable->End ((listSelf)))

/// Gets the number of elements in a map.
/// @param [in] listSelf pointer to map.
/// @returns the number of elements in the map.
#define SET_SIZE( listSelf ) \
  ((listSelf)->root ? BNODE_SIZE ((listSelf)->root) : 0)

/// Indicates a map is empty.
/// @param [in] listSelf pointer to map.
/// @returns 1 if the map is empty, 0 otherwise.
#define SET_IS_EMPTY( listSelf ) \
  (SET_SIZE ((listSelf)) == 0)

/// Typename of the template map.
/// @param [in] K typename of keys of elements hold in the map.
#define SET( K ) \
  SET_##K

#define SET_FIND3( map, begin, key ) \
  ((map)->root ? BNODE_FIND_KEY3((begin), (key), (map)->LessThan): SET_END (map))

#define SET_FIND2( map, key ) SET_FIND3( map, SET_BEGIN (map), key )

#define SET_FIND(...) VFUNC(SET_FIND, __VA_ARGS__)

#define SET_TRAVERSE4( map, callback, param, stop_condition ) \
  do { if ((map)->root) { \
         (map)->tree_locked = 1 ; \
         BNODE_TRAVERSE4((map)->root, (callback), (param), (stop_condition)) ; \
         (map)->tree_locked = 0 ; \
       } \
     } while (0)

#define SET_TRAVERSE3( map, callback, param ) SET_TRAVERSE4( map, callback, param, EXIT_FAILURE )

#define SET_TRAVERSE2( map, callback ) SET_TRAVERSE3( map, callback, map )

#define SET_TRAVERSE(...) VFUNC(SET_TRAVERSE, __VA_ARGS__)

#define SET_FOR_EACH6( map, begin, end, callback, param, stop_condition ) \
  ((map)->root ? (begin)->vtable->ForEach((begin), (end), (callback), (param), (stop_condition), BNODE_FORWARD) : SET_END (map))

#define SET_FOR_EACH5( map, begin, callback, param, stop_condition ) SET_FOR_EACH6( map, begin, SET_END (map), callback, param, stop_condition )

#define SET_FOR_EACH4( map, callback, param, stop_condition ) SET_FOR_EACH5( map, SET_BEGIN( map ), callback, param, stop_condition )

#define SET_FOR_EACH3( map, callback, param ) SET_FOR_EACH4( map, callback, param, EXIT_FAILURE )

#define SET_FOR_EACH2( map, callback ) SET_FOR_EACH3( map, callback, map )

#define SET_FOR_EACH(...) VFUNC(SET_FOR_EACH, __VA_ARGS__)

#define SET_INDEX( map, index) \
  ((map)->root ? BNODE_INDEX((map)->root, (index)) : (fprintf (stderr, "ERROR: " "Set is empty.\nABORT\n"), fflush (0), raise (SIGABRT), SET_END(map)))

#define SNODE( K ) \
  BNODE_##K##___set_dummy__

#define SNODE_PREVIOUS(n) BNODE_PREVIOUS(n)
#define SNODE_NEXT(n) BNODE_NEXT(n)
#define SNODE_COPY(n) BNODE_COPY(n)
#define SNODE_KEY(n) BNODE_KEY(n)

#endif
