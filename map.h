////////////////////////////////////////////////////
// Original Author: Laurent Farhi
// Date:   22/1/2017
// Contact: lfarhi@sfr.fr
//
// Based on design from Randy Gaul's Game Programming Blog http://cecilsunkure.blogspot.fr/2012/08/generic-programming-in-c.html
////////////////////////////////////////////////////

#pragma once

#ifndef __MAP_H__
#define __MAP_H__

#include "bnode.h"
#include "vfunc.h"

//
// DECLARE_MAP
// Purpose: Declares a linked list of a specific type on the heap.
//
#define DECLARE_MAP( K, T )     \
\
  DECLARE_BNODE(K, T)           \
\
  struct _MAP_##K##_##T;        \
\
  typedef struct _MAP_VTABLE_##K##_##T                                                  \
  {                                                                                     \
    BNODE_##K##_##T *(*CreateNode)( K key, int unique );                                \
    void (*Clear) ( struct _MAP_##K##_##T *self );                                      \
    void (*Destroy) ( struct _MAP_##K##_##T *self );                                    \
    BNODE_##K##_##T *(*Insert) ( struct _MAP_##K##_##T *self, BNODE_##K##_##T *node );  \
    int (*Remove) ( struct _MAP_##K##_##T *self, BNODE_##K##_##T *node );               \
    int (*Move) ( struct _MAP_##K##_##T *to, struct _MAP_##K##_##T *from, BNODE_##K##_##T *herefrom ); \
    BNODE_##K##_##T *(*Get) ( struct _MAP_##K##_##T *self, K key );                     \
    BNODE_##K##_##T *(*Set) ( struct _MAP_##K##_##T *self, K key, T value );            \
  } _MAP_VTABLE_##K##_##T;                                                              \
\
  typedef struct _MAP_##K##_##T             \
  {                                         \
    BNODE_##K##_##T *root;                  \
    const _MAP_VTABLE_##K##_##T *vtable;    \
    int (*LessThan) (K, K);                 \
    int (*LessThanValue) (T, T);            \
    int unique;                             \
    int tree_locked;                        \
  } MAP_##K##_##T;                          \
\
  MAP_##K##_##T *MAP_CREATE_##K##_##T( int (*less_than_operator) (K, K), int unique );  \

#define MAP_CREATE4( K, T, less_than_operator, unique ) \
  MAP_CREATE_##K##_##T((less_than_operator), (unique))

#define MAP_CREATE3( K, T, less_than_operator) MAP_CREATE4( K, T, less_than_operator, 1 )

#define MAP_CREATE2( K, T ) MAP_CREATE3( K, T, 0 )

/// Creates and returns a new map of specified type.
/// @param [in] K typename of keys of elements hold in the map.
/// @param [in] T typename of values of elements hold in the map.
/// @param [in, optional] less_than_operator Function defining the less than operator int (*less_than_operator) (K, K).
/// @param [in, optional] unique If set to 1 (by default), elements are unique in the map, otherwise they can be duplicated.
#define MAP_CREATE(...) VFUNC(MAP_CREATE, __VA_ARGS__)

/// Inserts a node of specified type in the map.
/// @param [in] listSelf Pointer to collection.
/// @param [in] key Key of type K to be inserted in the map.
/// @returns 1 in case of success, or 0 in case of failure (errno set to ENOMEM).
#define MAP_INSERT(listSelf, key) \
  ((listSelf)->vtable->Insert ((listSelf), (listSelf)->vtable->CreateNode ((key), (listSelf)->unique)))

/// Removes (deallocates) a node from a map.
/// @param [in] listSelf Pointer to map.
/// @param [in] node Pointer to the node to remove from the map.
#define MAP_REMOVE( listSelf, node ) \
  ((listSelf)->vtable->Remove ((listSelf), (node)))

/// Removes (deallocates) a collection and all its nodes.
/// @param [in] listSelf Pointer to collection.
#define MAP_DESTROY( listSelf ) \
  do { (listSelf)->vtable->Destroy ( (listSelf) ); } while(0)

/// Removes (deallocates) all the nodes from a collection.
/// @param [in] listSelf Pointer to collection.
#define MAP_CLEAR( listSelf ) \
  do { (listSelf)->vtable->Clear ( (listSelf) ); } while(0)

/// Moves an element from one collection to another.
/// @param [in] to pointer to the destination collection
/// @param [in] from pointer to the source collection
/// @param [in] herefrom Pointer to the node to move into the collection \p to
#define MAP_MOVE( to, from, herefrom ) \
  ((to)->vtable->Move ( (to), (from), (herefrom) ))

/// Gets the first node in a map.
/// @param [in] listSelf pointer to map.
/// @returns the first node in the map.
#define MAP_BEGIN( listSelf ) \
  ((listSelf)->root ? BNODE_FIRST ((listSelf)->root) : MAP_END( listSelf ))

/// Gets the last node in a map.
/// @param [in] listSelf pointer to map.
/// @returns the last node in the map.
#define MAP_LAST( listSelf ) \
  ((listSelf)->root ? BNODE_LAST ((listSelf)->root) : MAP_END( listSelf ))

/// Gets the one-past-last node in a map.
/// @param [in] listSelf pointer to map.
/// @returns the one-past-last node in the map.
#define MAP_END( listSelf ) \
  ((void*)0)

/// Gets the number of elements in a map.
/// @param [in] listSelf pointer to map.
/// @returns the number of elements in the map.
#define MAP_SIZE( listSelf ) \
  ((listSelf)->root ? BNODE_SIZE ((listSelf)->root) : 0)

/// Indicates a map is empty.
/// @param [in] listSelf pointer to map.
/// @returns 1 if the map is empty, 0 otherwise.
#define MAP_IS_EMPTY( listSelf ) \
  (MAP_SIZE ((listSelf)) == 0)

/// Typename of the template map.
/// @param [in] K typename of keys of elements hold in the map.
/// @param [in] T typename of values of elements hold in the map.
#define MAP( K, T ) \
  MAP_##K##_##T

#define MAP_SET( map, key, value ) \
  ((map)->vtable->Set((map), (key), (value)))

#define MAP_GET( map, key ) \
  ((map)->vtable->Get((map), (key)))

#define MAP_FIND_KEY3( map, begin, key ) \
  ((map)->root ? BNODE_FIND_KEY3((begin), (key), (map)->LessThan) : MAP_END (map))

#define MAP_FIND_KEY2( map, key ) MAP_FIND_KEY3( map, BNODE_FIRST((map)->root), key )

#define MAP_FIND_KEY(...) VFUNC(MAP_FIND_KEY, __VA_ARGS__)

#define MAP_FIND_VALUE3( map, begin, value ) \
  ((map)->root ? BNODE_FIND_VALUE3((begin), (value), (map)->LessThanValue) : MAP_END (map))

#define MAP_FIND_VALUE2( map, value ) MAP_FIND_VALUE3( map, BNODE_FIRST((map)->root), value )

#define MAP_FIND_VALUE(...) VFUNC(MAP_FIND_VALUE, __VA_ARGS__)

#define MAP_TRAVERSE4( map, callback, param, stop_condition ) \
  do { if ((map)->root) { \
         (map)->tree_locked = 1 ; \
         BNODE_TRAVERSE4((map)->root, (callback), (param), (stop_condition)) ; \
         (map)->tree_locked = 0 ; \
       } \
     } while (0)

#define MAP_TRAVERSE3( map, callback, param ) MAP_TRAVERSE4( map, callback, param, EXIT_FAILURE )

#define MAP_TRAVERSE2( map, callback ) MAP_TRAVERSE3( map, callback, map )

#define MAP_TRAVERSE(...) VFUNC(MAP_TRAVERSE, __VA_ARGS__)

#define MAP_FOR_EACH6( map, begin, end, callback, param, stop_condition ) \
  ((map)->root ? (begin)->vtable->ForEach((begin), (end), (callback), (param), (stop_condition), BNODE_FORWARD) : MAP_END (map))

#define MAP_FOR_EACH5( map, begin, callback, param, stop_condition ) MAP_FOR_EACH6( map, begin, MAP_END (map), callback, param, stop_condition )

#define MAP_FOR_EACH4( map, callback, param, stop_condition ) MAP_FOR_EACH5( map, MAP_BEGIN( map ), callback, param, stop_condition )

#define MAP_FOR_EACH3( map, callback, param ) MAP_FOR_EACH4( map, callback, param, EXIT_FAILURE )

#define MAP_FOR_EACH2( map, callback ) MAP_FOR_EACH3( map, callback, map )

#define MAP_FOR_EACH(...) VFUNC(MAP_FOR_EACH, __VA_ARGS__)

#define MAP_SET_LESS_THAN_VALUE_OPERATOR(map, lt)  \
  do { (map)->LessThanValue = (lt) ; } while(0)

#define MAP_INDEX( map, index) \
  ((map)->root ? BNODE_INDEX((map)->root, (index)): MAP_END (map))

#endif
