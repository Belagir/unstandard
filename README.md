# AN UNSTANDARD LIBRARY

This low-dependency C library provides a somewhat personal collection of code that is widely used in my own projects. It provides implementations for allocation, ranges, maths, testing...
By "personal", I mean that when presented with the same problem again and again while coding, I devised my same solution again and again ; taking inspiration from all places and people. So I created this library to organise all this mess, and here it is, the "unstandard" library.

## Quick overview

**allocation.h**

Grab and release memory through actual objects.

```c
struct allocator alloc = make_system_allocator();
u32 *array = alloc.malloc(alloc, sizeof(u32) * 42);
```

**logging.h**

Log messages to files / stdout with statically-allocated loggers.

```c
struct logger *err_logger = logger_create(stderr, LOGGER_ON_DESTROY_DO_NOTHING);
logger_log(err_logger, LOGGER_SEVERITY_INFO, "hello %s !", "world");
```

**range.h** & **sorting.h**

Manipulate contiguous collections of data, either on the stack or on the heap. Also provides heap sorting, dichotomic find, insert and removal.

```c
RANGE(u32) *array1 = &(RANGE(u32, 8)) RANGE_CREATE_STATIC(u32, 8u, { 5, 2, 1, 4, 3, });
printf("stack-allocated range of %d elements with a capacity of %d\n" array1.length, array1.capacity);
// stack-allocated range of 5 elements with a capacity of 8
RANGE(u32) *array2 = range_create_dynamic_from_copy_of(make_system_allocator(), RANGE_TO_ANY(array1));
printf("heap-allocated range of %d elements with a capacity of %d\n" array2.length, array2.capacity);
// heap-allocated range of 5 elements with a capacity of 8
```

**testutilities.h**

Macro machinery to create test scenarios, test cases, and run those cases.

```c
tst_CREATE_TEST_SCENARIO(some_test_scenario, { u32 input; }, { test_assert_equal(data->input, 42, "%d"); })

tst_CREATE_TEST_CASE(some_test_scenario_fail, some_test_scenario .input = 36 )
tst_CREATE_TEST_CASE(some_test_scenario_happy, some_test_scenario .input = 42 )

//...

tst_run_test_case(some_test_scenario_fail);
tst_run_test_case(some_test_scenario_happy);
```

And also :
 - **math.h**, **math2d.h**, and **math3d.h** that provide a good chunk of useful math functions and types ;
 - **res.h**, to fetch and name data that was embedded in the executable with the linker ;
 - **tree.h**, to manage statically-allocated n-trees ;
 - **unsignals.h**, to create and hook to global signals.

## Library Contents

The library grew organically, and some modules got more love than others, and some modules that got a lot of love didn't actually get any use. Here is a recap :

| `ustd/` header    | intended use                                                 | usefulness  | unit tests | used | personal appreciation                                        |
| ----------------- | ------------------------------------------------------------ | ----------- | ---------- | ---- | ------------------------------------------------------------ |
| `allocation.h`    | Manipulate allocator objects to make memory management manifest in function prototypes. | very high   | no         | yes  | I actually lost the tests for the static allocator 'ouroboros' (don't ask). |
| `common.h`        | Useful definitions and macros for basic stuff.               | very high   | no         | yes  | Included by every other header.                              |
| `logging.h`       | Create loggers in static data for lightweight and encapsulated logging. | high        | no         | yes  | The first module I created.                                  |
| `math.h`          | Some maths utilities I found myself using a lot.             | moderate    | no         | yes  | Not very extensive, might grow later.                        |
| `math2d.h`        | 2D vectors maths.                                            | moderate    | no         | yes  |                                                              |
| `math3d.h`        | 3D matrix and quaternion maths.                              | low         | yes        | yes  |                                                              |
| `range.h`         | Manage collections of data, either static or allocated.      | best        | yes        | yes  | The header I use the most. Not perfect by any means.         |
| `res.h`           | Associate symbols to data embedded into the executable.      | in question | no         | yes  | I have found a better way to store static data, that does imply to embed data in the executable. Will soon update the lib. |
| `sorting.h`       | Extends `range.h` to provide sorting and dichotomy over ranges. | high        | yes        | yes  |                                                              |
| `testutilities.h` | Macros to create test scenarios and test cases for unit testing. | very high   | i guess    | yes  | Once the few "gotchas" sorted, this provide a simple test framework. |
| `tree.h`          | Extends `range.h` to manage ranges as if they were n-tree structures. | low         | yes        | no   | The use case for this header is too limited : you need to use n-trees in an environment where allocation is forbidden. |
| `unsignals.h`     | Create, and (un)subscribe to program-wide signals.           | moderate    | no         | no   | Very recent.                                                 |

Additionally, I added a section to provide deeper access into some module(s) :

| `ustd_impl/` header | intended use                                                 | usefulness | tested | used | personal appreciation |
| ------------------- | ------------------------------------------------------------ | ---------- | ------ | ---- | --------------------- |
| `range_impl.h`      | Extends the capabilities of a range.                         | moderate   | yes    | yes  |                       |
| `unsignals_impl.h`  | Provides the implementation details needed to create signal subscriptions. | moderate   | no     | no   |                       |

## Dependencies

- C-23 compatible compiler^[1] ;

- `make`.


And the C standard library, of course. Tested on Linux.

> [1] : the makefile uses `gcc` as the compiler. You can overide this whan calling make : `make CC='your-favorite-compiler'`

## Usage

### Integration

Add this git repo as a submodule of your own project (or just clone it), and initialise it. Once this is done, `make` the library : this will yield a collection of `libunstandard.a` file in a new `unstandard/bin/` directory. Link against it with your favourite toolchain and include the headers present in `unstandard/inc/`, and you are good to go.

```bash
git submodule add git@github.com:Belagir/unstandard.git
# git clone git@github.com:Belagir/unstandard.git
make -C unstandard
ls -l unstandard/bin unstandard/inc
```

### Documentation

All of the headers' contents are decorated with Doxygen documentation. There is no recipe yet to generate actual documentation from those.

### Conventions

#### Coding rules

The library mainly uses the Linux Kernel coding rules, with some GNU coding rules here and there from the time when I was in the mood to try them.

#### Prefixes

All functions that relate to some data structure (or type) are prefixed by its symbol. No other namespacing is provided.

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

Most of the objects created by the library need clear lifetime procedures : a lot have a `*_create(...)` and a `*_destroy(...)` function associated with them. If a `*_create(...)` functions exists, then its `*_destroy(...)`  must also exist, and take a pointer to an object returned by the `*_create(...)` function. Use the `*_destroy(...)` function for each `*_create(...)` call !

All of the function's library that need to allocate dynamic memory must take a `struct allocator` as one of their parameters, and is discouraged to store it somewhere for later reuse, as it kinda breaks the whole principle behind it : passing allocators in arguments make memory management manifest at a glance.
