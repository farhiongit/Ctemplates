# Ctemplates --- templates for C
And templates for all ! Template containers (lists, sets and maps) for C language (why not ?).

(c) 2017 Laurent Farhi (lrspam at sfr.fr).

Templates are about generating code at compile time.
Languages such as C++ offer a powerful framework for template programming and usage.

Nevertheless, templates do not require runtime polymorphism, and can be implemented in C language. As such, Randy Gaul has demonstrated in 2012 [1] that the C macro preprocessor permits to implement a template mechanism in C.

The idea here is to exploit this feature to create template collections such as lists, sets and maps.

The solution is a combination of several techniques:

- macros to generate collection for any type and structure
- virtual tables to implement static polymorphism
- binary tree structure and operations
- function overloading Overloading on number of arguments (but not type though)

Let me know if you encounter any bug.

## Definition and declaration of template collections
Prior to any usage of template collections,

Header files should be included in source files :

- `#include "defops.h"`
- `#include "list_impl.h"`, `#include "set_impl.h"` or `#include "map_impl.h"` whether lists, sets or maps will be used.

Once header files are included, the template have to be instanciated for each type to be used in collections.

Templates are declared in global scope by:

    DECLARE_LIST (type)
    DECLARE_SET (type)
    DECLARE_MAP (key_type, value_type)

For each type, some helpers have to be defined to allow declaration od copy constructors, destructors and inequality operator `<`. This is done in global scope by:

    DEFINE_OPERATORS (type)

Templates are the defined in global scope by:

    DEFINE_LIST (type)
    DEFINE_SET (type)
    DEFINE_MAP (key_type, value_type)

If collections are intended to be used in a library,

- the library header file should include files defining the interface of template : `#include "list.h"`, `#include "set.h"` or `#include "map.h"` whether lists, sets or maps will be used.
- Declarations of templates should appear in the library header file.
- Definitions of operators and templates should appear in library source file.

## Lists
This container represents a strongly typed list of objects that can be accessed by index.

### Type
LIST(*T*), where *T* denotes the type contained in the list.

The type is denoted as *L* below.

### Elements
Elements are place-holders of values of type *T*.
Thoses elements allow to navigate through the container.
#### Element type
LNODE(*T*)

The type is denoted as *N* below.
#### Get value: *T* \*LNODE_VALUE(*N* \*n)
Dereferences the value of the element n.
The returned value should not be freed by the caller.
#### Assign value: void LNODE_ASSIGN(*N* \*n, *T* v)
Modifies the value hold be the element n with the value v..
A *copy* of T is assigned to the element.
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
DECLARE_LIST (T)
DEFINE_OPERATORS (T)
DEFINE_LIST (T)

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
SET(*K*), where *K* denotes the type contained in the set.

The type is denoted as *S* below.
### Unicity
Sets can either contain unique elements or not, depending on the parameters passed at creation.

### Elements
Elements are place-holders of values of type *K*.
Thoses elements allow to navigate through the container.
#### Element type
SNODE(*K*)

The type is denoted as *N* below.
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
##### **Description:** Applies a function `callback` to every element of the set sequentially, from beginning to end, until `callback` returns `until`. `callback` is called with element as the first argument. `param` is oassed as the second argument to each call of `callback`.
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
##### **Description:** Applies a function `callback` to every element of the set sequentially, from `begin` (included) to `end` (excluded), until `callback` returns `until`. `callback` is called with element as the first argument of `callback`. `param` is oassed as the second argument to each call of `callback`.
##### **Note:** The set *can be modified* (insertion or deletion of elements) during traversal.
##### **Return value:** None.
##### **Errors:** None.
##### **Complexity:** O(N *log* N)

#### `SET_BEGIN`
##### **Syntax:** *N* \*SET_BEGIN(*S* \*s)
##### **Description:** Yields the first element of a set.
##### **Return value:** First element of set `s`, or SET_END(l) if `s` is empty.
##### **Errors:** None.
##### **Complexity:** O(1)

#### `SET_LAST`
##### **Syntax:** *N* \*SET_LAST(*S* \*s)
##### **Description:** Yields the last element of a set.
##### **Return value:** Last element of set `s`, or SET_END(l) if `s` is empty.
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
DECLARE_SET (T)
DEFINE_OPERATORS (T)
DEFINE_SET (T)

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
This container represents a collection of keys and values that is maintained in sorted order by keys.
### Type
### Unicity

### Elements
#### Element type
#### Get key
#### Get value
#### Assign value
#### Move forward
#### Move backward

### Functions
#### `MAP_CREATE`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_DESTROY`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_INSERT`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 


#### `MAP_REMOVE`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_CLEAR`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_SIZE`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_IS_EMPTY`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_TRAVERSE`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_FOR_EACH`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_BEGIN`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_LAST`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_END`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_KEY`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_INDEX`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_FIND_KEY`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_FIND_VALUE`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_MOVE`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

#### `MAP_SET_VALUE`
##### **Syntax:** 
##### **Description:** 
##### **Return value:** 
##### **Errors:** 
##### **Complexity:** 

### Example
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
Sets, maps as well as `LIST_SORT` require a strict weak ordering.
Sets, maps as well as `LIST_FIND` and `LIST_UNIQUE` require equality operator.

For basic standard types, the standard operator `<` is defined and used by default.
For strings (`char *`), a default operator is defined from `strcoll`.
For other types, a default less than operator is defind as a bytewise comparator (something similar to `memcmp`).

_Optional_ operators can be assigned to user defined types managed by collections using:

- `SET_LESS_THAN_OPERATOR(T, operator)` where `operator` is a pointer to function with type `T (*operator) (T arg1, T arg2)`. This function should return 1 if `arg1` < `arg2`, 0 otherwise.

`GET_LESS_THAN_OPERATOR(T)` returns the pointer to the less than operator previously defined for type T.

For convenience, `LESS_THAN_OPERATOR_TYPE(T)` is the predefined type for pointer function to less than operator for type *T*.

Two values `arg1` and `arg2` are considered to be equal when neither `arg1` < `arg2`, nor `arg1` > `arg2`.

For sets and maps, less than operators can also be optionally specified when the collection is created (see `SET_CREATE` and `MAP_CREATE`). If not, the less than operator tied to type *T* will be used.

For lists, a less than operator can also be specified as the optional second argument of `LIST_SORT` and `LIST_UNIQUE`. If not, the less than operator tied to type *T* will be used.

## Algorithmic complexity



|                     | List | Set | Map |
|:--------------------|:----:|:---:|:---:|
|Insert               | O(log N) | O(log N) | O(log N) |
|Remove               |      |     |     |
|Move                 |      |     |     |
|Next                 |      |     |     |
|Index                | O(log N) |     |     |
|Previous             |      |     |     |
|Go to begining       |      |     |     |
|Go to end            |      |     |     |
|Go to index          |      |     |     |
|Search key           |      |     |     |
|Search value         |      |     |     |
|Traverse             |      |     |     |
|For_each             |      |     |     |
|Swap                 |      |     |     |
|Reverse              |      |     |     |
|Sort                 |      |     |     |
|Rotate left or right |      |     |     |
|Clear or destroy     |      |     |     |

Lists, sets and maps are internally constructed and organized upon binary trees.
Those complexities rely on perfectly balanced trees.

## Tree self-balancing

Internal data in lists, sets and maps are organized into self-balancing binary trees.
This allows to reduce algorithmic complexity to O(*log* N) for most operations.

The self-balacing strategy is relaxed:

- it is applied at insertion, in order to keep the branch into which the element is inserted not longer than the sibling breanch.
- it is not applied when an element is removed. The branch where the element is removed can get shorter than its sibling branch.

This strategy garanties than the tree depth is always lower or equal to (*log* Nmax)/(*log* 2), where Nmax is the larger size of the collections.

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

[1] Randy Gaul's Game Programming Blog - Generic Programming in C (http://www.randygaul.net/2012/08/10/generic-programming-in-c/)

[2] The C Preprocessor - Duplication of Side Effects (https://gcc.gnu.org/onlinedocs/cpp/Duplication-of-Side-Effects.html#Duplication-of-Side-Effects)
