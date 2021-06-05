
## Types in Variables
Implement variable types that can store types themselves. This is currently possible in say Python, as seen in the following example:
```python
my_type = float
my_int  = 5
> my_type(my_int)
>>> 5.0 
```

This features should also allow for "dynamic" typecasting or initialization of variables by doing say:
```c
type my_type = float;
int my_age   = 42;
my_type age_as_float = (my_type) my_age;
>>> age_as_float: float == 5.0
```
Another example:
```c
/* example.erd */
type my_type = int;
my_type age = (my_type) argv[1];


$ ./example.erd "42"
>>> age: int == 42
```
Be aware of run-time errors!



## Callables As Types
As in many other languages, we should have a type dedicated to storing callables.  This is not to promote OOP but moreso to make a nicer version of C's function pointers.

Example:
```c
define struct DATASET {
    array byte data;
    callable reader;
}

define str reader(bytes data) {
    /* Convert bytes into some proper string that it represents */
    return ...

}

/* malloc is not neccesarily in this language, but this is just an example */
DATASET * housing = (DATASET) malloc(sizeof(DATASET));

housing->reader = reader;
housing->data = ...

str read_data = housing->reader(housing->data);
>>> read_data: str == ...
```

## Lambdas
Just as most other languages, we want to support anonymous functions.  Currently it is expected to look a little like this:

Example 1:
```c
callable addr = lambda (x,y,z) = {x + y + z}
int result = addr(1,2,3);
>>> result: int == 6
```

Example 2:
```c
array int values = [1,4,5,2,7,1];
array int evens  = filter(
    values, 
    lambda (x) = {x % 2 == 0}
) 
```

## Pointers, Kinda. 
We're big fans of pointers being available to developers. However, the combination of lacking error-handling and very direct memory addressing in C makes this not only too dangereous but also sligty limited.

Our goal is to implement pointers as _functions_, which we could then (inside a stdlib, say pointers.erd) implement with extra functionality.

The syntax might be the same as seen in C, but work as macros that our preprocessor then replaces with calls to the pointers.erd functions.