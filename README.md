# Ctemplates --- templates for C
And templates for all ! Template containers (lists, sets and maps) for C language (why not ?).

Templates are about generating code at compile time.
Languages such as C++ offer a powerful framework for template programming and usage.
Nevertheless, Randy Gaul has demonstrated in 2012 [1] that the C macro preprocessor permits to implement a template mechanism in C.

The idea here is to exploit this feature to create template collections such as lists, sets and maps.

The solution is a combination of several techniques:

- macros to generate collection for any type and structure
- virtual tables to implement static polymorphism
- binary tree structure and operations
- function overloading Overloading on number of arguments (but not type though)

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

## Operators
_Optional_ operators can be assigned to types managed by collections:

- `SET_DESTRUCTOR(type, destructor)` where `destructor` is a pointer to function with type `void (*destructor) (type arg)`
- `SET_COPY_CONSTRUCTOR(type, constructor)` where `constructor` is a pointer to function with type `type (*constructor) (type arg)`
- `SET_LESS_THAN_OPERATOR(type, operator)` where `operator` is a pointer to function with type `type (*operator) (type arg1, type arg2)`. This function should return 1 if `arg1` < `arg2`, 0 otherwise.

For basic standard types, standard operators are defined and used by default.

## Lists

## Sets

## Maps

## Algorithmic complexity

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
Look at examples in directory `examples`.

[1] Randy Gaul's Game Programming Blog - Generic Programming in C (http://www.randygaul.net/2012/08/10/generic-programming-in-c/)

[2] The C Preprocessor - Duplication of Side Effects (https://gcc.gnu.org/onlinedocs/cpp/Duplication-of-Side-Effects.html#Duplication-of-Side-Effects)
