# EMT-MATH Project State

Last verified: 2026-09-24

## Mission

EMT-MATH is a from-scratch ISO C99 linear algebra and numerical-computing
library. It is a learning project that connects mathematics, algorithms,
memory ownership, data layout, testing, numerical stability, and performance.
It will later become the mathematical foundation for simulation and a physics
engine.

## Current milestone

Vector lifecycle is implemented and tested.

Current public types and functions:

```c
typedef enum {
    OK = 0,
    ERROR_NULL_PTR = -1,
    ERROR_INVALID_STATE = -2,
    ERROR_SIZE_OVERFLOW = -3,
    ERROR_ALLOCATION = -4
} VectorStatus;

typedef struct {
    size_t size;
    double *data;
} Vector;

VectorStatus initVector(Vector *vector, size_t size);
void destroyVector(Vector *vector);
```

The public naming is not frozen. `Vector` was deliberately chosen by the user;
other names may be reviewed but must not be silently changed.

## Ownership model

- The caller owns the `Vector` struct, normally on the stack.
- The vector owns its heap-allocated `data` buffer.
- A new vector is initialized as `Vector vector = {0};`.
- `initVector` accepts only an empty vector.
- `destroyVector` frees the buffer and restores the empty state.
- There is no global memory manager or pool.
- Shallow copying a live `Vector` is unsafe; a future copy operation must be a
  deep copy.

## Lifecycle implementation

`initVector` uses validate–prepare–commit:

- validates pointer and empty state;
- handles requested size zero without allocation;
- checks multiplication overflow before `malloc`;
- allocates into a temporary pointer;
- initializes elements to `0.0`;
- commits fields only after success.

`destroyVector` is null-safe and repeatable for a valid empty vector.

## Build

- GCC with `-std=c99 -Wall -Wextra -Wpedantic -g3 -O0`.
- Public headers are found through `CPPFLAGS := -Iinclude`.
- `main.c` and `vector.c` compile into separate object files.
- Test sources matching `tests/test_*.c` become separate executables.
- `make test` runs every discovered test executable.

## Tests

`tests/test_vector.c` covers:

1. successful allocation, size, and zero initialization;
2. requested size zero;
3. null vector pointer;
4. rejected reinitialization without mutation;
5. allocation-size multiplication overflow;
6. reset after destroy;
7. null and repeated destroy.

All tests passed at the last verification. AddressSanitizer and
UndefinedBehaviorSanitizer found no address or UB error with leak detection
disabled. Valgrind currently cannot start because the system loader lacks the
debug symbols required for mandatory function redirection.

## Repository state at last verification

The repository had one initial commit:

```text
b801bf0 project initialized and Makefile created
```

The Makefile changes, vector module, tests, and `compile_flags.txt` were still
uncommitted. Always re-check with `git status` before work.

## Immediate next decision

Review public naming before the API grows, then implement bounded element
access with explicit null, state, bounds, `const`, and output-parameter
semantics. See `docs/OFFLINE_PLAN.md` for task cards.

