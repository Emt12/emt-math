# EMT-MATH Decision Log

Only user-approved architectural decisions belong here. Proposed changes stay
in handoff or roadmap documents until accepted.

## D-001 — ISO C99

Status: accepted

EMT-MATH targets ISO C99. Builds use `-std=c99` together with strict warnings.

## D-002 — Runtime dependencies

Status: accepted

The implementation uses the C standard library; `libm` is allowed. External
linear-algebra and physics libraries are not used. Development tools such as
compilers, debuggers, sanitizers, and profilers are allowed.

## D-003 — Vector ownership

Status: accepted

The caller owns the `Vector` struct. A live vector owns its heap buffer.
Initialization allocates the buffer; destruction frees the buffer and resets
the struct. No central registry or pool is used for the linear-algebra layer.

## D-004 — Empty state

Status: accepted

The canonical empty vector is:

```c
size == 0
data == NULL
```

Caller-created vectors begin as `Vector vector = {0};`. A successful destroy
restores this state.

## D-005 — Library error reporting

Status: accepted

Library functions return status codes. They do not print, log, or terminate the
process. The caller, test executable, or application decides how to report an
error.

## D-006 — Zero initialization

Status: accepted

Allocated `double` elements are assigned `0.0` explicitly. The project does not
depend on all-bits-zero being the portable representation of floating-point
zero.

## Open decisions

- Freeze or revise public function and status naming before the API expands.
- Exact contracts for bounded element access.
- Whether zero-length vectors remain valid operands for each mathematical
  operation.

