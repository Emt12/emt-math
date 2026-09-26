# EMT-MATH Offline Coding Plan

This file is designed for sessions without internet access. Work from the top
and stop at each gate. Do not jump to matrix code until the vector contracts
are stable.

## Before leaving internet access

- [ ] Confirm the repository is available locally.
- [ ] Run `make clean`, `make`, and `make test` once.
- [ ] Confirm `gcc`, `make`, `gdb`, and `git` work offline.
- [ ] Open `docs/ROADMAP.md`, `docs/DECISIONS.md`, and this file once in Neovim.
- [ ] Commit or stash unrelated work; do not mix experiments accidentally.
- [ ] Ensure laptop power/build tools are ready.

## Useful offline commands

```sh
make
make test
git status --short
git diff
git diff --check
gcc --version
gdb ./build/test_vector
```

For one-off sanitizer verification:

```sh
gcc -std=c99 -Wall -Wextra -Wpedantic -g3 -O0 \
    -fsanitize=address,undefined -fno-omit-frame-pointer \
    -Iinclude tests/test_vector.c src/vector/vector.c \
    -o build/test_vector_sanitized
```

Then run:

```sh
ASAN_OPTIONS=detect_leaks=0 ./build/test_vector_sanitized
```

Leak detection is disabled here only because it previously failed under the
Codex sandbox. On the user's normal terminal, try it without that override.

## Session 0 — Re-establish a clean baseline

Goal: know exactly what currently works.

- [ ] Read `include/emt/vector.h`.
- [ ] Read `src/vector/vector.c`.
- [ ] Read `tests/test_vector.c`.
- [ ] Explain out loud who owns the struct and who owns `data`.
- [ ] Run a clean build and all tests.
- [ ] Inspect `git diff --check` for whitespace errors.
- [ ] Note, but do not automatically fix, naming or style preferences.

Gate:

```text
Build succeeds with no warning.
All lifecycle tests pass.
Current Git diff is understood.
```

## Session 1 — Freeze public naming

Goal: make one deliberate naming decision before the API expands.

Review these current names:

```text
EmtVector
EmtVectorStatus
VEC_OK
ERROR_NULL_PTR
emt_vec_init
emt_vec_destroy
```

Questions to answer in notes:

1. Could `VEC_OK` or `EmtVector` collide with another C header?
2. Is the preferred function style `emt_vec_init` or `vector_init`?
3. Should status constants be namespaced, for example `VECTOR_OK`?
4. Is EMT-MATH intended as a small personal project or an includable public
   library?
5. Is consistency with future `Matrix` APIs more important than brevity?

Output:

- [ ] Write the chosen naming rule into `docs/DECISIONS.md`.
- [ ] If names change, change header, source, and tests together.
- [ ] Rebuild and rerun tests.

Do not perform a half-rename.

## Session 2 — Design bounded access, no implementation

Goal: write the contract before code.

Design a setter and getter. Do not copy a ready-made signature without
answering:

- Is a null vector an error?
- What makes a vector state valid?
- Is `index == size` valid? Why not?
- How does a getter return both status and `double`?
- Should a getter promise not to mutate the vector?
- If the output pointer is null, what status is returned?
- For a zero-length vector, what result does any index produce?

Output:

- [ ] Candidate declarations written in a notebook or temporary comment.
- [ ] A new out-of-bounds status selected.
- [ ] Test table written before implementation.

Suggested test table:

| Case | Expected result |
|---|---|
| set index `0` in size `3` | success |
| set index `2` in size `3` | success |
| set index `3` in size `3` | out of bounds |
| get first/last index | correct value |
| null vector | null-pointer error |
| null getter output | null-pointer error |
| empty vector, index `0` | out of bounds |

## Session 3 — Implement setter

Goal: one small mutation API.

- [ ] Add only the accepted status and setter declaration to the header.
- [ ] Implement null, state, and bounds validation.
- [ ] Mutate exactly one element on success.
- [ ] Do not print or allocate.
- [ ] Add setter tests from the table.
- [ ] Confirm failed calls do not mutate the vector.
- [ ] Run all tests.

Complexity target:

```text
time: O(1)
additional space: O(1)
```

## Session 4 — Implement getter and learn `const`

Goal: safely read one element while distinguishing errors from valid numeric
values.

- [ ] Use `const` on the vector input if the contract permits.
- [ ] Return the value through an output pointer.
- [ ] Validate before writing to the output.
- [ ] Decide whether the output remains unchanged on failure and test it.
- [ ] Add first, last, out-of-bounds, null, and empty tests.
- [ ] Run all tests.

Explain after implementation:

- Why returning only `double` makes error reporting ambiguous.
- Why `const EmtVector *` does not make the pointed-to allocation globally
  immutable forever; it restricts mutation through that access path.

## Session 5 — Fill operation

Goal: write the first linear traversal.

- [ ] Design behavior for null, invalid, and empty vectors.
- [ ] Set every element to a supplied scalar.
- [ ] Test positive, negative, fractional, and empty cases.
- [ ] State complexity.

Expected analysis:

```text
time: O(n)
additional space: O(1)
```

## Session 6 — Deep copy

Goal: make ownership consequences concrete.

Questions:

- Must destination begin empty?
- Who allocates destination storage?
- What happens if allocation fails?
- Can source and destination be the same object?
- How do tests prove buffers do not alias?

Required tests:

- [ ] sizes and values match;
- [ ] `source.data != destination.data` for a non-empty copy;
- [ ] changing destination does not change source;
- [ ] empty-source copy;
- [ ] null pointers;
- [ ] invalid/non-empty destination according to the chosen contract;
- [ ] both vectors are destroyed without double-free.

## Session 7 — Addition API design

Goal: design the first mathematical operation before implementation.

Mathematics:

```text
c_i = a_i + b_i
```

Decisions:

- Does the operation allocate `result`, or must caller initialize it?
- Is an empty destination required?
- Are `result == a` or `result == b` allowed?
- What happens on dimension mismatch?
- What state is guaranteed after failure?

Write tests before code:

- simple three-element example;
- negative and fractional values;
- dimension mismatch;
- null arguments;
- empty vectors;
- aliasing cases according to the chosen policy;
- destination unchanged on failure if promised.

## Session 8 — Implement addition

- [ ] Add the selected dimension-mismatch status.
- [ ] Validate all arguments before mutation/allocation.
- [ ] Implement the loop.
- [ ] Run the written tests.
- [ ] State `O(n)` time and space behavior.
- [ ] Run sanitizer verification.

## Session 9 — Subtraction and scalar multiplication

Reuse the established addition conventions rather than inventing a different
ownership model for each operation.

- [ ] Subtraction mathematics and tests.
- [ ] Scalar multiplication mathematics and tests.
- [ ] Confirm aliasing policy.
- [ ] Confirm zero-length policy.
- [ ] Run all previous tests to detect regressions.

## Session 10 — Dot product

Mathematics:

```text
a · b = Σ a_i b_i
```

- [ ] Design output/error reporting.
- [ ] Test orthogonal, parallel, negative, and fractional vectors.
- [ ] Test dimension mismatch.
- [ ] Document naive accumulation and rounding limitations.
- [ ] Do not prematurely add SIMD or threading.

Explain:

```text
a · b = ||a|| ||b|| cos(theta)
```

## Session 11 — Norm and distance

- [ ] Implement naive Euclidean norm first.
- [ ] Link with `libm` using the Makefile linker flags.
- [ ] Use tolerance-based tests.
- [ ] Test zero vector and known `3-4-5` vector.
- [ ] Implement distance using the mathematical definition without unnecessary
  persistent allocation.
- [ ] Document overflow/underflow risk in the naive sum of squares.

Later improvement, not required immediately: scaled sum-of-squares norm.

## Session 12 — Projection and normalization

- [ ] Detect zero-norm denominator.
- [ ] Normalize a nonzero vector.
- [ ] Project one vector onto another.
- [ ] Test orthogonal, parallel, and zero-vector cases.
- [ ] Explain the geometry rather than only the loop.

## Session 13 — EmtVector milestone review

- [ ] Run all tests.
- [ ] Run sanitizer build.
- [ ] Review every public function for consistent names and contracts.
- [ ] Review failure atomicity and aliasing.
- [ ] Write vector API documentation.
- [ ] Record known numerical limitations.
- [ ] Commit the finished vector milestone.

Only after this gate should matrix lifecycle start.

## If stuck offline

Use this debugging sequence:

1. Read the first compiler error only.
2. Verify the exact declaration in the header.
3. Verify the definition matches it exactly.
4. Check include spelling and case.
5. Reduce the failing test to one operation.
6. Print sizes, indices, and status codes in the test/application—not inside
   the library.
7. Use GDB to stop at the failing function.
8. Re-read ownership: who allocated, who frees, and is the object initialized?
9. Check whether a multiplication overflowed before validation.
10. After fixing, run the entire test suite, not only the failing test.

## Notes to bring back to Codex/Claude

At the end of an offline session, record:

```text
Completed:
Tests run:
Compiler warnings:
Design decisions made:
Uncertain questions:
Current git status:
```

Put factual state changes into `docs/ai/PROJECT_STATE.md`. Put unresolved
questions into the appropriate handoff file. Put only accepted architecture
choices into `docs/DECISIONS.md`.

