# Ctemplates --- templates for C
And templates for all ! Template containers (lists, sets and maps) for C language (why not ?).

(c) 2017 Laurent Farhi (lrspam at sfr.fr).

Dreaming of templates for C, such as

```c
LIST (int) * mylist = LIST_CREATE (int);
LIST_APPEND (mylist, 2);
LIST_APPEND (mylist, 1);
LIST_SORT (mylist);
```
?

Make your dream come true, read on !

This project is about turning theoretical ideas of Randy Gaul on [his excellent blog](http://www.randygaul.net/2012/08/10/generic-programming-in-c/) into operational template containers.

Templates are about generating code at compile time.
Languages such as C++ offer a powerful framework for template programming and usage.

Nevertheless, templates do not require runtime polymorphism, and can be implemented in C language. As such, Randy Gaul has demonstrated in 2012 [1] that the C macro preprocessor permits to implement a template mechanism in C.

The idea here is to exploit this feature to create template collections such as lists, sets and maps.

The solution is a combination of several techniques:

- macros to generate collection for any type and structure
- virtual tables to implement static polymorphism
- binary tree structure and operations
- function overloading on number of arguments (but not type though)

Let me know if you encounter any bug, or have any comment or suggestion.

## Definition and declaration of template collections
Prior to any usage of template collections, header files should be included in source files :

- `#include "defops.h"`
- `#include "list_impl.h"`, `#include "set_impl.h"` or `#include "map_impl.h"` whether lists, sets or maps will be used.

Once header files are included, the template have to be instanciated for each type to be used in collections.

Templates are **declared** in global scope by:

    DECLARE_LIST (type);
    DECLARE_SET (type);
    DECLARE_MAP (key_type, value_type);

For each type, some helpers have to be defined to allow declaration of copy constructors, destructors and inequality operator `<`.
This is done in global scope by:

    DEFINE_OPERATORS (type);

Templates **are defined** in global scope by:

    DEFINE_LIST (type);
    DEFINE_SET (type);
    DEFINE_MAP (key_type, value_type);

If collections are intended to be used in a library,

- the library header file should include files defining the interface of template : `#include "list.h"`, `#include "set.h"` or `#include "map.h"` whether lists, sets or maps will be used.
- Declarations of templates should appear in the library header file.
- Definitions of operators and templates should appear in library source file.

Three common types of containers are provided: lists, sets and maps.

## Lists
This container represents a strongly typed list of objects that can be accessed by index.

### Type
The type of a list is LIST(*T*), where *T* denotes the type contained in the list.

This type is noted as *L* below.

### Elements
Elements are place-holders of values of type *T*.
Thoses elements allow to navigate through the container.
#### Element type
The type of the elements of a list is LNODE(*T*).

This type is noted as *N* below.
#### Get value: *T* \*LNODE_VALUE(*N* \*n)
Dereferences the value of the element n.
The returned value should not be freed by the caller.
#### Assign value: void LNODE_ASSIGN(*N* \*n, *T* v)
Modifies the value hold be the element n with the value v..
A *copy* of v is assigned to the element.
#### Move forward: *N* \*LNODE_NEXT(*N* \*n)
Returns a pointer to the next element in the list or LIST_END(*L*). Complexity: O(*log* N)
#### Move backward: *N* \*LNODE_PREVIOUS(*N* \*n)
Returns a pointer to the previous elemnt in the list or LIST_END(*L*). Complexity: O(*log* N)

### Functions
#### `LIST_CREATE`
##### **Syntax:** *L* \*LIST_CREATE(*T*)
##### **Description:** Creates a new list. This list must be destroyed by LIST_DESTROY after usage.
##### **Return value:** A pointer to the created list, or 0 in case of memory allocation error.
##### **Errors:** ENOMEM Out of memory.
##### **Complexity:** O(1)

#### `LIST_DESTROY`
##### **Syntax:** void LIST_DESTROY(*L* \*l)
##### **Description:** Desallocate all elements in the list and desallocate the list. The destructor of type *T* is called for each element of the list. The list must not be used afterwards.
##### **Return value:** None
##### **Errors:** None
##### **Complexity:** O(N)

#### `LIST_INSERT`
##### **Syntax:** *N* \*LIST_INSERT(*L* \*l, *N* \*n, *T* v)
##### **Description:** Inserts an element containing the value v before the element n where n is an element of the list l or LIST_END(l). The new element is inserted at the end of the list if n is equal to LIST_END(l).
##### **Return value:** A pointer to the created element in case of success, or 0 otherwise.
##### **Errors:** EINVAL if n is not LIST_END(l) and is not an element of the list l. ENOMEM in case of memory allocation error.
##### **Complexity:** O(*1og* N)

#### `LIST_APPEND`
##### **Syntax:** *N* \*LIST_APPEND(*L* \*l, *T* v)
##### **Description:** Inserts an element containing the value v at the end of the list. Behaves as well as LIST_INSERT(l, LIST_END(l), v)
##### **Return value:** A pointer to the created element in case of success, or 0 otherwise.
##### **Errors:** ENOMEM in case of memory allocation error.
##### **Complexity:** O(*1og* N)

#### `LIST_REMOVE`
##### **Syntax:** int LIST_REMOVE(*L* \*l, *N* \*n)
##### **Description:** Removes the element n from the list l. The value hold by the eleemnt is destroyed. n should not be dereferenced afterwards.
##### **Return value:** EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
##### **Errors:** EINVAL if n is not an element of the list l.
##### **Complexity:** O(1)

#### `LIST_CLEAR`
##### **Syntax:** void LIST_CLEAR(*L* \*l)
##### **Description:** Desallocate all elements in the list. The destructor of type *T* is called for each element of the list. The list can still be used afterwards.
##### **Return value:** None
##### **Errors:** None
##### **Complexity:** O(N)

#### `LIST_SIZE`
##### **Syntax:** size_t LIST_SIZE(*L* \*l)
##### **Description:** Returns the number of elements in list l.
##### **Return value:** The number of elements in list l
##### **Errors:** None
##### **Complexity:** O(1)

#### `LIST_IS_EMPTY`
##### **Syntax:** int LIST_IS_EMPTY(*L* \*l)
##### **Description:** Indicates if the list is empty.
##### **Return value:** 1 if the list is empty, 0 otherwise
##### **Errors:** None
##### **Complexity:** O(1)

#### `LIST_TRAVERSE`
##### **Syntax:**
###### void LIST_TRAVERSE(*L* \*l, int (\*callback)( *N* \*, void \*))
###### void LIST_TRAVERSE(*L* \*l, int (\*callback)( *N* \*, void \*), void \*param)
###### void LIST_TRAVERSE(*L* \*l, int (\*callback)( *N* \*, void \*), void \*param, int until)
Arguments `param` and `until` are optional. Default values are respectively l and EXIT_FAILURE.
##### **Description:** Applies a function `callback` to every element of the list sequentially, from beginning to end, until `callback` returns `until`. `callback` is called with element as the first argument. `param` is oassed as the second argument to each call of `callback`.
##### **Note:** The list must not be modified (insertion or deletion of elements) during traversal.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(N)

#### `LIST_FOR_EACH`
##### **Syntax:**
###### void LIST_FOR_EACH(*L* \*l, int (\*callback)(*N* \*, void \*))
###### void LIST_FOR_EACH(*L* \*l, int (\*callback)(*N* \*, void \*), void \*param)
###### void LIST_FOR_EACH(*L* \*l, int (\*callback)(*N* \*, void \*), void \*param, int until)
###### void LIST_FOR_EACH(*L* \*l, *N* \*begin, int (\*callback)(*N* \*, void \*), void \*param, int until)
###### void LIST_FOR_EACH(*L* \*l, *N* \*begin, *N* \*end, int (\*callback)(*N* \*, void \*), void \*param, int until)
Arguments `param`, `until`, `begin` and `end` are optional. Default values are respectively l, EXIT_FAILURE, LIST_BEGIN(l) and LIST_END(l).
##### **Description:** Applies a function `callback` to every element of the list sequentially, from `begin` (included) to `end` (excluded), until `callback` returns `until`. Elements are passed sequentially as the first argument of `callback`. `param` is oassed as the second argument to each call of `callback`.
##### **Note:** The list *can be modified* (insertion or deletion of elements) during traversal.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(N *log* N)

#### `LIST_BEGIN`
##### **Syntax:** *N* \*LIST_BEGIN(*L* \*l)
##### **Description:** Yields the first element of a list.
##### **Return value:** First element of list l, or LIST_END(l) if l is empty.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `LIST_LAST`
##### **Syntax:** *N* \*LIST_LAST(*L* \*l)
##### **Description:** Yields the last element of a list.
##### **Return value:** Last element of list l, or LIST_END(l) if l is empty.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `LIST_END`
##### **Syntax:** *N* \*LIST_END(*L* \*l)
##### **Description:** Yields the past-the-end element of a list.
##### **Note:** The returned element must not be dereferenced.
##### **Return value:** Past-the-end element of list l.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `LIST_INDEX`
##### **Syntax:** *N* \*LIST_INDEX(*L* \*l, size_t index)
##### **Description:** Yields the *i*th element of a list.
##### **Return value:** The *i*th element of list l, *i* is a 0-based index.
##### **Errors:** None.
##### **Note:** l should not be empty and index should be positive and strictly less than LIST_SIZE(l).
##### **Complexity:** O(*1og* N)

#### `LIST_FIND`
##### **Syntax:** 
##### *N* \*LIST_FIND(*L* \*l, *T* v)
##### *N* \*LIST_FIND(*L* \*l, *T* v, int (*op)(T, T))
##### *N* \*LIST_FIND(*L* \*l, *T* v, int (*op)(T, T))
##### *N* \*LIST_FIND(*L* \*l, *N* \*begin, *T* v, int (*neq)(T, T))
Arguments `op` and `begin` are optionnal. Default values are respectively the default less than operator and LIST_BEGIN(l).
##### **Description:** Finds the first element n in list l after element begin (included) which holds a value equal to v, that is for which op(value of n, v) and op(v, value of n) return 0.
##### **Note:** op(*T* a, *T* b) should either
- return 0 if the two values are equal, 1 otherwise ;
- return 1 if `a` is strictly less than `b`, 0 otherwise.
##### **Return value:** The first element n in list l after element begin (included) which holds a value equal to v, that is for which op(value of n, v) and op(v, value of n) return 0, or LIST_END(l) if no occurrence were found.
##### **Errors:** EINVAL if begin does not belong to l.
##### **Complexity:** O(N *log* N)

#### `LIST_MOVE`
##### **Syntax:** int LIST_MOVE(*L* \*to, *N* \*hereto, *L* \*from, *N* \*herefrom)
##### **Description:** Moves element `herefrom` of list `from` before element `hereto` of list `to`.
##### **Return value:** EXIT_SUCCESS in case of success, EXIT_FAILURE otherwise.
##### **Errors:** EINVAL if `herefrom` does not belong to the list `from` or if `hereto` is not LIST_END(to) and is not an element of the list `to`.
##### **Complexity:** O(*1og* N)

#### `LIST_SWAP`
##### **Syntax:** int LIST_SWAP(*L* \*la, *N* \*na, *L* \*lb, *N* \*nb)
##### **Description:** Swaps elements `na` and `nb` between lists `la` and `lb`.
##### **Return value:** EXIT_SUCCESS in case of success, EXIT_FAILURE otherwise. 
##### **Errors:** EINVAL if `na` does not belong to `la`, or if `nb` does not belong to `lb`.
##### **Complexity:** O(*1og* N)

#### `LIST_REVERSE`
##### **Syntax:** 
###### int LIST_REVERSE(*L* \*l)
###### int LIST_REVERSE(*L* \*l, *N* \*begin, *N* \*end)
##### **Description:** Reverses the order of the elements of the list l. If `begin` and `end` are specified, only the elements between `begin` (included) and `end` (excluded) are reversed.
##### **Note:** `end` should be forward reachable from `begin`.
##### **Return value:** EXIT_SUCCESS in case of success, EXIT_FAILURE otherwise.
##### **Errors:** EINVAL if `begin` or `end` do not belong to `l`.
##### **Complexity:** O(N *1og* N)

#### `LIST_ROTATE_LEFT` and `LIST_ROTATE_RIGHT`
##### **Syntax:**
###### void LIST_ROTATE_LEFT(*L* \*l)
###### void LIST_ROTATE_RIGHT(*L* \*l)
##### **Description:** Rotates all the elements of the list one position to the left (`LIST_ROTATE_LEFT`) or to the right (`LIST_ROTATE_RIGHT`).
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(*1og* N)

#### `LIST_SORT`
##### **Syntax:**
###### void LIST_SORT(*L* \*l)
###### void LIST_SORT(*L* \*l, int (*lt)(T, T))
##### **Description:** Sorts the elements of the list in ascending order with respect to the less than operator `lt`.
##### **Note:** lt(*T* a, *T* b) should return 1 if `a` is strictly less than `b`, 0 otherwise. If `lt` is not specified, the standard less than operator is used.
##### **Return value:** None.
##### **Errors:** ENOMEM in case of memory allocation error.
##### **Complexity:** Complexity of `qsort`

#### `LIST_UNIQUE`
###### size_t LIST_UNIQUE(*L* \*l)
###### size_t LIST_UNIQUE(*L* \*l, int (*op)(T, T))
##### **Description:** Removes all but one adjacent matching elements (which values are equal with respect to `op`) from list l. Only the *first* occurrence is kept in the list.
##### **Note:** op(*T* a, *T* b) should either
- return 0 if the two values are equal, 1 otherwise ;
- return 1 if `a` is strictly less than `b`, 0 otherwise.

If `op` is not specified, the standard less than operator is used.
##### **Return value:** The number of elements removed from the list.
##### **Errors:** None.
##### **Complexity:** O(N *1og* N)

### Example
```c
typedef char *T;
DECLARE_LIST (T);
DEFINE_OPERATORS (T);
DEFINE_LIST (T);

int main (void)
{
  LIST (T) * mylist = LIST_CREATE (T);

  LIST_APPEND (mylist, "A");
  LIST_INSERT (mylist, LIST_LAST (mylist), "aaa");
  LIST_INSERT (mylist, LIST_BEGIN (mylist), "bbbb");
  LIST_INSERT (mylist, 1), "f");

  LIST_SORT (mylist);
  LIST_REVERSE (mylist);

  LNODE_ASSIGN (LIST_BEGIN (mylist), "zzzz");

  LIST_DESTROY (mylist);
}
```
Look at a complete [example](examples/list_example.c).

## Sets
This container represents a collection of objects that is maintained in sorted order.
### Type
The type of a set is SET(*K*), where *K* denotes the type contained in the set.

This type is noted as *S* below.
### Unicity
Sets can either contain unique elements or not, depending on the third parameters passed at creation (`SET_CREATE`).

### Elements
Elements are place-holders of values of type *K*.
Thoses elements allow to navigate through the container.
#### Element type
The type of the elements of a set is SNODE(*K*)

This type is noted as *N* below.
#### Get value: *K* \*SNODE_KEY(*N* \*n)
Dereferences the value of the element n.
The returned value should not be freed by the caller.
#### Move forward: *N* \*SNODE_NEXT(*N* \*n)
Returns a pointer to the next element in the list or SET_END(*L*). Complexity: O(*log* N)

#### Move backward: *N* \*SNODE_PREVIOUS(*N* \*n)
Returns a pointer to the previous elemnt in the list or SET_END(*L*). Complexity: O(*log* N)

### Functions
#### `SET_CREATE`
##### **Syntax:** *S* \* SET_CREATE(*K*, [int (*less_than_operator) (K, K)=0], [int unicity=1])
Parameters between square bracket are optional.
##### **Description:** Creates a new set. This set must be destroyed by SET_DESTROY after usage.
##### **Return value:** A pointer to the created set, or 0 in case of memory allocation error.
##### **Errors:** ENOMEM Out of memory.
##### **Complexity:** O(1)

#### `SET_DESTROY`
##### **Syntax:** void SET_DESTROY(*S* \*s)
##### **Description:** Desallocates all elements in the set and desallocates the set. The destructor of type *K* is called for each element of the set. The set must not be used afterwards.
##### **Return value:** None
##### **Errors:** None
##### **Complexity:** O(N)

#### `SET_INSERT`
##### **Syntax:** *N* \*SET_INSERT(*S* \*s, *K* k)
##### **Description:** Inserts an element containing the value k
##### **Note:** No insertion is performed if an element with the same value is already in the set, and unicity is required.
##### **Return value:** A pointer to the created element in case of success, or 0 otherwise.
##### **Errors:** ENOMEM in case of memory allocation error.
##### **Complexity:** O(*1og* N)

#### `SET_REMOVE`
##### **Syntax:** int SET_REMOVE(*S* \*s, *N* \*n)
##### **Description:** Removes the element n from the set `s`. The value hold by the eleemnt is destroyed. `n` should not be dereferenced afterwards.
##### **Return value:** EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
##### **Errors:** EINVAL if `n` is not an element of the set `s`.
##### **Complexity:** O(1)

#### `SET_CLEAR`
##### **Syntax:** void SET_CLEAR(*S* \*s)
##### **Description:** Desallocates all elements in the set. The destructor of type *K* is called for each element of the set. The set can still be used afterwards.
##### **Return value:** None
##### **Errors:** None
##### **Complexity:** O(N)

#### `SET_SIZE`
##### **Syntax:** size_t SET_SIZE(*S* \*s)
##### **Description:** Returns the number of elements in set `s`.
##### **Return value:** The number of elements in set `s`
##### **Errors:** None
##### **Complexity:** O(1)

#### `SET_IS_EMPTY`
##### **Syntax:** int SET_IS_EMPTY(*S* \*s)
##### **Description:** Indicates if the set is empty.
##### **Return value:** 1 if the set is empty, 0 otherwise
##### **Errors:** None
##### **Complexity:** O(1)

#### `SET_TRAVERSE`
##### **Syntax:**
###### void SET_TRAVERSE(*S* \*s, int (\*callback)( *N* \*, void \*))
###### void SET_TRAVERSE(*S* \*s, int (\*callback)( *N* \*, void \*), void \*param)
###### void SET_TRAVERSE(*S* \*s, int (\*callback)( *N* \*, void \*), void \*param, int until)
Arguments `param` and `until` are optional. Default values are respectively `s` and EXIT_FAILURE.
##### **Description:** Applies a function `callback` to every element of the set sequentially, from beginning to end, until `callback` returns `until`. `callback` is called with element as the first argument. `param` is passed as the second argument to each call of `callback`.
##### **Note:** The set must not be modified (insertion or deletion of elements) during traversal.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(N)

#### `SET_FOR_EACH`
##### **Syntax:**
###### void SET_FOR_EACH(*S* \*s, int (\*callback)(*N* \*, void \*))
###### void SET_FOR_EACH(*S* \*s, int (\*callback)(*N* \*, void \*), void \*param)
###### void SET_FOR_EACH(*S* \*s, int (\*callback)(*N* \*, void \*), void \*param, int until)
###### void SET_FOR_EACH(*S* \*s, *N* \*begin, int (\*callback)(*N* \*, void \*), void \*param, int until)
###### void SET_FOR_EACH(*S* \*s, *N* \*begin, *N* \*end, int (\*callback)(*N* \*, void \*), void \*param, int until)
Arguments `param`, `until`, `begin` and `end` are optional. Default values are respectively `s`, EXIT_FAILURE, SET_BEGIN(s) and SET_END(s).
##### **Description:** Applies a function `callback` to every element of the set sequentially, from `begin` (included) to `end` (excluded), until `callback` returns `until`. `callback` is called with element as the first argument of `callback`. `param` is passed as the second argument to each call of `callback`.
##### **Note:** The set *can be modified* (insertion or deletion of elements) during traversal.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(N *log* N)

#### `SET_BEGIN`
##### **Syntax:** *N* \*SET_BEGIN(*S* \*s)
##### **Description:** Yields the first element of a set.
##### **Return value:** First element of set `s`, or SET_END(s) if `s` is empty.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `SET_LAST`
##### **Syntax:** *N* \*SET_LAST(*S* \*s)
##### **Description:** Yields the last element of a set.
##### **Return value:** Last element of set `s`, or SET_END(s) if `s` is empty.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `SET_END`
##### **Syntax:** *N* \*SET_END(*S* \*s)
##### **Description:** Yields the past-the-end element of a set.
##### **Note:** The returned element must not be dereferenced.
##### **Return value:** Past-the-end element of set `s`.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `SET_KEY`
##### **Syntax:** *N* \*SET_KEY(*S* \*s, *K* k)
##### **Description:** Returns the first element which value is equal to k.
##### **Return value:** The first element which value is equal to k, or SET_END(s) if no element are found.
##### **Errors:** None.
##### **Complexity:** O(*log* N)

#### `SET_INDEX`
##### **Syntax:** *N* \*SET_INDEX(*S* \*s, size_t index)
##### **Description:** Yields the *i*th element of a set.
##### **Return value:** The *i*th element of set `s`, *i* is a 0-based index.
##### **Errors:** None.
##### **Note:** `s` should not be empty and index should be positive and strictly less than SET_SIZE(s).
##### **Complexity:** O(*1og* N)

#### `SET_FIND`
##### **Syntax:** *N* \*SET_FIND(*S* \*s, [*N* \*n=SET_BEGIN(s)], *K* k)
The second argument is optional.
##### **Description:** Returns the first element after n (included) which value is equal to k.
##### **Return value:** The first element after n (included) which value is equal to k.
##### **Errors:** None.
##### **Complexity:** O(*1og* N) if the second argument is not specified, O(N *1og* N) otherwise.

#### `SET_MOVE`
##### **Syntax:** int SET_MOVE(*S* \*to, *S* \*from, *N* \*herefrom)
##### **Description:** Moves element `herefrom` of set `from` into set `to`.
##### **Return value:** EXIT_SUCCESS in case of success, EXIT_FAILURE otherwise.
##### **Errors:** EINVAL if `herefrom` does not belong to the set `from`.
##### **Complexity:** O(*1og* N)

### Example
```c
typedef char *T;
DECLARE_SET (T);
DEFINE_OPERATORS (T);
DEFINE_SET (T);

int main (void)
{
  SET (T) * myset = SET_CREATE (T);
  SET_INSERT (myset, "b");
  SET_INSERT (myset, "c");
  SET_INSERT (myset, "a");
  SET_DESTROY (myset);
}
```
Look at a complete [example](examples/set_example.c).

## Maps
This container represents a collection of pairs of keys and values that is maintained in sorted order of keys.
### Type
The type of a map is MAP(*K*, *T*), where *K* denotes the key type contained in the map, and *T* the value type assiciated to the keys.

This type is noted as *M* below.
### Unicity
Maps can either contain unique elements or not, depending on the third parameters passed at creation (`MAP_CREATE`).

### Elements
Elements are place-holders of pairs (key, value) of type (*K*, *T*).
Thoses elements allow to navigate through the container.
#### Element type
The type of the elements of a map is BNODE(*K*, *T*)

This type is noted as *N* below.
#### Get key: *K* \*BNODE_KEY(*N* \*n)
Dereferences the key of the element n.
The returned value should not be freed by the caller.
#### Get value: *T* \*BNODE_VALUE(*N* \*n)
Dereferences the value of the element n.
The returned value should not be freed by the caller.
#### Assign value: void BNODE_ASSIGN(*N* \*n, *T* v)
Modifies the value hold be the element n with the value v..
A *copy* of v is assigned to the element.

#### Move forward: *N* \*BNODE_NEXT(*N* \*n)
Returns a pointer to the next element in the list or MAP_END(*L*). Complexity: O(*log* N)
#### Move backward: *N* \*BNODE_PREVIOUS(*N* \*n)
Returns a pointer to the previous elemnt in the list or MAP_END(*L*). Complexity: O(*log* N)

### Functions
#### `MAP_CREATE`
##### **Syntax:** *M* \* MAP_CREATE(*K*, *T*, [int (*less_than_operator) (K, K)=0], [int unicity=1])
Parameters between square bracket are optional.
##### **Description:** Creates a new map of pairs (*K*, *T*). This map must be destroyed by MAP_DESTROY after usage.
##### **Return value:** A pointer to the created map, or 0 in case of memory allocation error.
##### **Errors:** ENOMEM Out of memory.
##### **Complexity:** O(1)

#### `MAP_DESTROY`
##### **Syntax:** void MAP_DESTROY(*M* \*m)
##### **Description:** Desallocates all elements in the map and desallocates the map. The destructor of types *K* and *T* are called for each element of the map
##### **Note:** The map must not be used destruction.
##### **Return value:** None
##### **Errors:** None
##### **Complexity:** O(N)

#### `MAP_INSERT`
##### **Syntax:** *N* \*MAP_INSERT(*M* \*m, *K* k, *T* v)
##### **Description:** Inserts an element containing the key-value pair (k, v).
##### **Note:** No insertion is performed if an element with the same key is already in the map, and unicity is required.
##### **Return value:** A pointer to the created element in case of success, or 0 otherwise.
##### **Errors:** ENOMEM in case of memory allocation error.
##### **Complexity:** O(*1og* N)

#### `MAP_SET_VALUE`
##### **Syntax:** *N* \*MAP_SET_VALUE(*M* \*m, *K* k, *T* v)
##### **Description:** Modifies the value associated with the key k in the map : the associated value is set to the value v.
##### **Note:** An element is inserted if no element with the same key is already in the map, or if unicity is not required.
##### **Return value:** A pointer to the modiied element in case of success, or 0 otherwise.
##### **Errors:** ENOMEM in case of memory allocation error.
##### **Complexity:** O(*1og* N)

#### `MAP_REMOVE`
##### **Syntax:** int MAP_REMOVE(*M* \*m, *N* \*n)
##### **Description:** Removes the element n from the map `m`. The pair hold by the element is destroyed. `n` should not be dereferenced afterwards.
##### **Return value:** EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
##### **Errors:** EINVAL if `n` is not an element of the map `m`.
##### **Complexity:** O(1)

#### `MAP_CLEAR`
##### **Syntax:** void MAP_CLEAR(*S* \*s)
##### **Description:** Desallocates all elements in the map. The destructors of types *K* and *T* are called for each element of the map. The map can still be used afterwards.
##### **Return value:** None
##### **Errors:** None
##### **Complexity:** O(N)

#### `MAP_SIZE`
##### **Syntax:** size_t MAP_SIZE(*M* \*m)
##### **Description:** Returns the number of elements in map `m`.
##### **Return value:** The number of elements in map `m`
##### **Errors:** None
##### **Complexity:** O(1)

#### `MAP_IS_EMPTY`
##### **Syntax:** int MAP_IS_EMPTY(*M* \*m)
##### **Description:** Indicates if the map is empty.
##### **Return value:** 1 if the map is empty, 0 otherwise
##### **Errors:** None
##### **Complexity:** O(1)

#### `MAP_TRAVERSE`
##### **Syntax:**
###### void MAP_TRAVERSE(*M* \*m, int (\*callback)( *N* \*, void \*))
###### void MAP_TRAVERSE(*M* \*m, int (\*callback)( *N* \*, void \*), void \*param)
###### void MAP_TRAVERSE(*M* \*m, int (\*callback)( *N* \*, void \*), void \*param, int until)
Arguments `param` and `until` are optional. Default values are respectively `m` and EXIT_FAILURE.
##### **Description:** Applies a function `callback` to every element of the map sequentially, from beginning to end, until `callback` returns `until`. `callback` is called with element as the first argument. `param` is passed as the second argument to each call of `callback`.
##### **Note:** The map must not be modified (insertion or deletion of elements) during traversal.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(N)

#### `MAP_FOR_EACH`
##### **Syntax:**
###### void MAP_FOR_EACH(*M* \*m, int (\*callback)(*N* \*, void \*))
###### void MAP_FOR_EACH(*M* \*m, int (\*callback)(*N* \*, void \*), void \*param)
###### void MAP_FOR_EACH(*M* \*m, int (\*callback)(*N* \*, void \*), void \*param, int until)
###### void MAP_FOR_EACH(*M* \*m, *N* \*begin, int (\*callback)(*N* \*, void \*), void \*param, int until)
###### void MAP_FOR_EACH(*M* \*m, *N* \*begin, *N* \*end, int (\*callback)(*N* \*, void \*), void \*param, int until)
Arguments `param`, `until`, `begin` and `end` are optional. Default values are respectively `m`, EXIT_FAILURE, MAP_BEGIN(m) and MAP_END(m).
##### **Description:** Applies a function `callback` to every element of the map sequentially, from `begin` (included) to `end` (excluded), until `callback` returns `until`. `callback` is called with element as the first argument of `callback`. `param` is passed as the second argument to each call of `callback`.
##### **Note:** The map *can be modified* (insertion or deletion of elements) during traversal.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(N *log* N)

#### `MAP_BEGIN`
##### **Syntax:** *N* \*MAP_BEGIN(*M* \*m)
##### **Description:** Yields the first element of a map.
##### **Return value:** First element of map `m`, or MAP_END(m) if `m` is empty.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `MAP_LAST`
##### **Syntax:** *N* \*MAP_LAST(*M* \*m)
##### **Description:** Yields the last element of a map.
##### **Return value:** Last element of map `m`, or MAP_END(m) if `m` is empty.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `MAP_END`
##### **Syntax:** *N* \*MAP_END(*M* \*m)
##### **Description:** Yields the past-the-end element of a map.
##### **Note:** The returned element must not be dereferenced.
##### **Return value:** Past-the-end element of map `m`.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `MAP_KEY`
##### **Syntax:** *N* \*MAP_KEY(*M* \*m, *K* k)
##### **Description:** Returns the first element which key is equal to k.
##### **Return value:** The first element which key is equal to k, or MAP_END(m) if no element are found.
##### **Errors:** None.
##### **Complexity:** O(*log* N)

#### `MAP_INDEX`
##### **Syntax:** *N* \*MAP_INDEX(*M* \*m, size_t index)
##### **Description:** Yields the *i*th element of a map.
##### **Return value:** The *i*th element of map `m`, *i* is a 0-based index.
##### **Errors:** None.
##### **Note:** `m` should not be empty and index should be positive and strictly less than MAP_SIZE(s).
##### **Complexity:** O(*1og* N)

#### `MAP_FIND_KEY`
##### **Syntax:** *N* \*MAP_FIND_KEY(*M* \*m, [*N* \*n=MAP_BEGIN(m)], *K* k)
The second argument is optional.
##### **Description:** Returns the first element after n (included) which key is equal to k.
##### **Return value:** The first element after n (included) which key is equal to k.
##### **Errors:** None.
##### **Complexity:** O(*1og* N) if the second argument is not specified, O(N *1og* N) otherwise.

#### `MAP_FIND_VALUE`
##### **Syntax:** *N* \*MAP_FIND_VALUE(*M* \*m, [*N* \*n=MAP_BEGIN(m)], *T* v)
The second argument is optional.
##### **Description:** Returns the first element after n (included) which value is equal to v.
##### **Return value:** The first element after n (included) which value is equal to v.
##### **Errors:** None.
##### **Complexity:** O(N *1og* N).

#### `MAP_SET_LESS_THAN_VALUE_OPERATOR`
##### **Syntax:** void MAP_SET_LESS_THAN_VALUE_OPERATOR(*M* \*m, int (*less_than_operator) (T, T))
##### **Description:** Defines a less than operator tied to the map `m`.
##### **Note:** If no operator is specified for the map `m`, the operator attached to type *T* is used, or the default less than operator for type *T*.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(1).

#### `MAP_MOVE`
##### **Syntax:** int MAP_MOVE(*M* \*to, *M* \*from, *N* \*herefrom)
##### **Description:** Moves element `herefrom` of map `from` into map `to`.
##### **Return value:** EXIT_SUCCESS in case of success, EXIT_FAILURE otherwise.
##### **Errors:** EINVAL if `herefrom` does not belong to the map `from`.
##### **Complexity:** O(*1og* N)

### Example
```c
typedef struct
{
  int l, w, h;
} Dimensions;
DEFINE_OPERATORS (pchar);
DEFINE_OPERATORS (Dimensions);

DECLARE_MAP (pchar, Dimensions);
DEFINE_MAP (pchar, Dimensions);

static int
print_car (BNODE (pchar, Dimensions) * car, void *arg)
{
  (void) arg;
  printf ("%s (%d, %d, %d)\n", *BNODE_KEY (car), BNODE_VALUE (car)->l, BNODE_VALUE (car)->w, BNODE_VALUE (car)->h);

  return EXIT_SUCCESS;
}

int
main (void)
{
  MAP (pchar, Dimensions) * cars = MAP_CREATE (pchar, Dimensions);

  Dimensions rt = {.l = 3595,.w = 1647,.h = 1557 };
  Dimensions cc1 = {.l = 3466,.w = 1615,.h = 1460 };
  Dimensions p108 = {.l = 3475,.w = 1615,.h = 1460 };
  MAP_INSERT (cars, "Renault Twingo", cc1);     // Inserts and sets value
  MAP_SET_VALUE (cars, "Renault Twingo", rt);   // Does not insert but sets value
  MAP_SET_VALUE (cars, "Citroën C1", cc1);     // Inserts and sets value
  MAP_INSERT (cars, "Citroën C1", rt); // Does neither insert nor modify value
  MAP_SET_VALUE (cars, "Peugeot 108", cc1);     // Inserts and sets value
  MAP_SET_VALUE (cars, "Peugeot 108", p108);    // Does not insert but sets value

  MAP (pchar, Dimensions) * fiat = MAP_CREATE (pchar, Dimensions);

  Dimensions mini3 = {.l = 3821,.w = 1727,.h = 1415 };
  MAP_SET_VALUE (fiat, "Mini Cooper", mini3);
  Dimensions f500 = {.l = 3546,.w = 1627,.h = 1488 };
  MAP_SET_VALUE (fiat, "Fiat 500", f500);

  MAP_MOVE (cars, fiat, MAP_KEY (fiat, "Fiat 500"));
  MAP_REMOVE (fiat, MAP_KEY (fiat, "Mini Cooper"));
  printf ("%lu elements in fiat\n", MAP_SIZE (fiat));

  MAP_DESTROY (fiat);

  MAP_TRAVERSE (cars, print_car);

  // Find keys in the set
  char *alicia[2] = { "Fiat 500", "Mini Cooper" };
  for (size_t i = 0; i < sizeof (alicia) / sizeof (*alicia); i++)
    if (MAP_FIND_KEY (cars, alicia[i]))
      printf ("%s is in cars.\n", alicia[i]);
    else
      printf ("%s is NOT in cars.\n", alicia[i]);

  Dimensions d = {.l = 3546,.w = 1627,.h = 1488 };

  BNODE (pchar, Dimensions) * c = MAP_FIND_VALUE (cars, d);
  if (c)
    printf ("%s\n", *BNODE_KEY (c));

  c = MAP_INDEX (cars, 1);
  if (c)
    printf ("%s\n", *BNODE_KEY (c));

  c = MAP_LAST (cars);
  if (c)
    printf ("%s\n", *BNODE_KEY (c));

  MAP_DESTROY (cars);
}
```
Look at a complete [example](examples/map_example.c).

## Memory managment
Collections can manage their own memory for the data held into the elements.
For basic standard types, the default copy constructor is the `=` operator.
For strings (`char *`), the default constructor is defined as `strdup` and the default destructor is defined as `free`.

For user defined types *T*, _optional_ operators can be assigned to types contained into collections using:

- `SET_COPY_CONSTRUCTOR(T, constructor)` where `constructor` is a pointer to function with type `T (*constructor) (T v)`
- `SET_DESTRUCTOR(T, destructor)` where `destructor` is a pointer to function with type `void (*destructor) (T v)`

Theses operators can be reset to their default value by passing 0 to `SET_DESTRUCTOR` and `SET_COPY_CONSTRUCTOR`.

`GET_DESTRUCTOR(T)` and `GET_COPY_CONSTRUCTOR(T)` allow to return pointers to previously defined destructor and copy constructor for type *T*.

For convenience, `COPY_CONSTRUCTOR_TYPE(T)` and `DESTRUCTOR_TYPE(T)` are the predefined types for pointer functions to copy constructor and destructor for type *T*. For instance, one can thus write

```c
COPY_CONSTRUCTOR_TYPE(T) cptor = GET_COPY_CONSTRUCTOR(T);
```

## Ordering and equality
### Ordering
Sets, maps as well as `LIST_SORT` require a strict weak ordering.
Sets, maps as well as `LIST_FIND` and `LIST_UNIQUE` require equality operator.

For basic standard types, the standard operator `<` is defined and used by default.
For strings (`char *`), a default operator is defined from `strcoll`.
For other types, a default less than operator is defind as a bytewise comparator (something similar to `memcmp`).

User defined operators can _optionally_ replace default operators using:

- `SET_LESS_THAN_OPERATOR(T, operator)` where `operator` is a pointer to function with type `T (*operator) (T arg1, T arg2)`. This function should return 1 if `arg1` < `arg2`, 0 otherwise.

`GET_LESS_THAN_OPERATOR(T)` returns the pointer to the less than operator previously defined for type T.

For convenience, `LESS_THAN_OPERATOR_TYPE(T)` is the predefined type for pointer function to less than operator for type *T*.

For sets and maps, less than operators can also be optionally specified when the collection is created (see `SET_CREATE` and `MAP_CREATE`). If not, the less than operator tied to type *T* will be used (either the user defined or defualt one).

For lists, a less than operator can also be specified as the optional second argument of `LIST_SORT` and `LIST_UNIQUE`. If not, the less than operator tied to type *T* will be used (either the user defined or defualt one).

### Equality
Equality is required for key or data matching (using finctions `...FIND...`) and unicity (during insertion or for `LIST_UNIQUE`).

Two values `arg1` and `arg2` are considered to be equal when neither `arg1` < `arg2`, nor `arg1` > `arg2`.

## Tree self-balancing

Internal data in lists, sets and maps are organized into self-balancing binary trees.
This allows to reduce algorithmic complexity to O(*log* N) for most operations.

The self-balacing strategy is relaxed:

- it is applied at insertion, in order to keep the branch into which the element is inserted not longer than the sibling breanch.
- it is not applied when an element is removed. The branch where the element is removed can get shorter than its sibling branch.

This strategy garanties than the tree depth is always lower or equal to (*log* Nmax)/(*log* 2), where Nmax is the larger size of the collections.

## Algorithmic complexity
The complexity in worst case is compared in the following table.

|                     | List | Set | Map |
|:--------------------|:----:|:---:|:---:|
|Insert               | O(log N) | O(log N) | O(log N) |
|Remove               | O(1) | O(1) | O(1) |
|Move                 | O(log N) | O(log N) | O(log N) |
|Index                | O(log N) | O(log N) | O(log N) |
|Go to Previous       | O(log N) | O(log N) | O(log N) |
|Go to Next           | O(log N) | O(log N) | O(log N) |
|Go to begining       | O(1) | O(1) | O(1) |
|Go to end            | O(1) | O(1) | O(1) |
|Go to index          | O(log N) | O(log N) | O(log N) |
|Search key           | - | O(log N) | O(log N) |
|Search value         | O(N) | - | O(N) |
|Traverse             | O(N) | O(N) | O(N) |
|For_each             | O(N log N) | O(N log N) | O(N log N) |
|Clear or destroy     | O(N) | O(N) | O(N) |

## Tips and pitfalls
Template declarations can not apply directly on compound types such as `char *`, `unsigned long` or `struct foo`.
User defined types should be used to create template collections on such compound types.

    typedef unsigned long ulong;
    typedef struct foo foo;
    typedef char * pchar; 

Evaluated variables should not be used for the first parameter of template functions.
Since mecro are extensively used, this would otherwise have unintended side effects [2] (at least as long as GNU C language extensions `__auto` and Statements and Declarations in Expressions are not part of the standard C language.)

Avoid

    LNODE_ASSIGN(LIST_BEGIN(list), 55);
    
Prefer

    LNODE(int) b = LIST_BEGIN(list);
    LNODE_ASSIGN(b, 55);

## Examples
Look at examples in directory [`examples`](examples).

`gol.c` is a naïve, slow and conventional implementation (neither pattern recognition nor time compression,
as in hashlife algorithm) of the Conway's Game Of Life using lists, sets and maps.

## Tests
Lists, sets and maps functionnalities have been thoroughly tested as well as self-balancing algorithm.
`valgrind` has been used to check the execution of thoses tests is memory leajs free.

Look at full tests in directory [`test`](test).

## References
[1] Randy Gaul's Game Programming Blog - Generic Programming in C (http://www.randygaul.net/2012/08/10/generic-programming-in-c/)

[2] The C Preprocessor - Duplication of Side Effects (https://gcc.gnu.org/onlinedocs/cpp/Duplication-of-Side-Effects.html#Duplication-of-Side-Effects)
