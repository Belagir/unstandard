# AN UNSTANDARD LIBRARY

This low-dependency C library provides a somewhat personal collection of code that is widely used in my own projects. It provides implementations for allocation, ranges, maths, testing...
By "personal", I mean that when presented with the same problem again and again while coding, I devised my same solution again and again ; taking inspiration from all places and people. So I created this library to organise all this mess, and here it is, the unstandard library.

## Library Contents

The library grew organically, and some modules got more love than others, and some modules that got a lot of love didn't actually get any use. Here is a recap :

| `ustd/` header    | intended use                                                 | usefulness  | unit tests | used | personal appreciation                                        |
| ----------------- | ------------------------------------------------------------ | ----------- | ---------- | ---- | ------------------------------------------------------------ |
| `allocation.h`    | Manipulate allocator objects to make memory management manifest in function prototypes. | very high   | no         | yes  | I actually lost the tests for the static allocator 'ouroboros' (don't ask). |
| `common.h`        | Useful definitions and macros for basic stuff.               | very high   | no         | yes  | Included by every other header.                              |
| `logging.h`       | Create loggers in static data for lightweight and encapsulated logging. | high        | no         | yes  | The first module I created.                                  |
| `math2d.h`        | 2D vectors maths.                                            | moderate    | no         | yes  |                                                              |
| `math3d.h`        | 3D matrix and quaternion maths.                              | low         | yes        | yes  |                                                              |
| `math.h`          | Some maths utilities I found myself using a lot.             | moderate    | no         | yes  | Not very extensive, might grow later.                        |
| `range.h`         | Manage collections of data, either static or allocated.      | best        | yes        | yes  | The header I use the most. Not perfect by any means.         |
| `res.h`           | Associate symbols to data embedded into the executable.      | in question | no         | yes  | I have found a better way to store static data, that does imply to embed data in the executable. Will soon update the lib. |
| `sorting.h`       | Extends `range.h` to provide sorting and dichotomy over ranges. | high        | yes        | yes  |                                                              |
| `testutilities.h` | Macros to create test scenarios and test cases for unit testing. | very high   | i guess    | yes  | Once the few "gotchas" sorted, this provide a simple test framework. |
| `tree.h`          | Extends `range.h` to manage ranges as if they were n-tree structures. | low         | yes        | no   | The use case for this header is too limited : you need to use n-trees in an environment where allocation is forbidden. |

Additionally, I added a section to provide deeper access into some module(s) :

| `ustd_impl/` header | intended use                        | usefulness | tested | used | personal appreciation |
| ------------------- | ----------------------------------- | ---------- | ------ | ---- | --------------------- |
| `range_impl.h`      | Extend the capabilities of a range. | moderate   | yes    | yes  |                       |

## Dependencies

- C-23 compatible compiler (`gcc-13` is) ;

- `make`.


And the C standard library, of course. Tested on Linux.

## Usage

### Integration

Add this git repo as a submodule of your own project, and initialise it. Once this is done, `make` the library : this will yield a collection of `*.o` files in a new `unstandard/build/` folder. Link against them and include the headers present in `unstandard/inc/` with your preferred toolchain, and you are good to go. Someday I will add the capability to bundle this as a library. Someday.

### Documentation

All of the headers' contents are decorated with Doxygen documentation. There is no recipe yet to generate actual documentation from those.

### Conventions

#### Coding rules

The library mainly uses the Linux Kernel coding rules, with some GNU coding rules here and there when I was in the mood to try them. 

#### Prefixes

All functions that relate to some data structure (or type) are prefixed by its symbol. 

```c
// Example :

typedef struct logger logger;

logger * logger_create(FILE target[static 1], logger_on_destroy on_destroy);
void logger_destroy(logger **logger);
void logger_log(logger *logger, logger_severity severity, char *msg, ...);

// ...
```

Other than that, not every symbol has a prefix, though everything in the library endeavours to have a very descriptive name.

#### Memory and lifetime

Most of the objects created by the library need clear lifetime procedures : a lot have a `*_create(...)` and a `*_destroy(...)` function associated with them. If a `*_create(...)` functions exists, then its `*_destroy(...)`  must also exist, and take a pointer to an object returned by the `*_create(...)` function.

All of the function's library that need to allocate dynamic memory must take a `struct allocator` as one of their parameters, and is discouraged to store it somewhere for later reuse.
