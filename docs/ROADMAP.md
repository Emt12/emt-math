# EMT-MATH Roadmap

This roadmap is ordered by dependency, not by spectacle. Every phase must
produce explainable mathematics, a documented C contract, tests, and—when
performance matters—measurements.

## Project-wide definition of done

A feature is complete only when all applicable items are true:

- The mathematical definition and assumptions are understood.
- Input, output, ownership, aliasing, and error behavior are explicit.
- The implementation compiles as ISO C99 with all configured warnings.
- Normal cases and edge cases have tests.
- Floating-point tests use an appropriate tolerance.
- Allocation paths have no known leak or invalid access.
- Complexity and numerical-stability limitations are documented.
- Benchmarks are added only after correctness is established.
- Public API or architecture changes are recorded in `docs/DECISIONS.md`.

## Phase 0 — C and project foundation

Status: mostly complete

### Knowledge

- structs, pointers, pointer-to-pointer;
- `malloc`, `free`, `size_t`, `const`;
- header/source separation;
- object files and linking;
- Make targets, prerequisites, automatic variables, and pattern rules;
- ownership, lifetime, empty state, and transactional initialization.

### Deliverables

- [x] Repository layout.
- [x] C99 debug build with strict warnings.
- [x] `compile_flags.txt` for clangd.
- [x] Separate application and library object files.
- [x] Automatic discovery and execution of test files.
- [ ] Sanitizer build target.
- [ ] Release build target.
- [ ] Static library target (`libemtmath.a`) when more than one module exists.
- [ ] Select a license before public release.

### Exit gate

The user can explain compilation versus linking, ownership of every allocation,
and why an object must be initialized before its fields are read.

## Phase 1 — EmtVector core

Status: lifecycle complete; operations pending

### 1.1 Lifecycle

- [x] Empty state.
- [x] Overflow-safe allocation.
- [x] Zero initialization.
- [x] Destruction and reset.
- [x] Lifecycle tests.
- [ ] Freeze public naming before adding many functions.
- [ ] Commit the lifecycle milestone.

### 1.2 Bounded element access

Concepts:

- `const` correctness;
- input versus output pointers;
- index bounds;
- valid versus invalid object state;
- status codes without printing.

Deliverables:

- [+] Design getter and setter contracts.
- [+] Add an out-of-bounds status.
- [+] Decide whether getters use an output parameter.
- [+] Test first index, last index, and `index == size`.
- [+] Test null vector and null output pointer.
- [+] Decide and test zero-length behavior.

### 1.3 Utility operations

- [+] Fill every element with one value.
- [+] Deep copy.
- [ ] Swap two vectors without allocation.
- [ ] Optional resize only after its failure semantics are designed.

Important questions:

- Is destination allocation performed by the operation or by the caller?
- What happens when source and destination are the same object?
- On failure, must the destination remain unchanged?

### 1.4 Arithmetic

- [+] Addition.
- [+] Subtraction.
- [ ] Scalar multiplication.
- [ ] Dot product.
- [ ] Euclidean norm.
- [ ] Distance.

Mathematics:

```text
(a + b)_i = a_i + b_i
a · b = Σ a_i b_i
||a||₂ = sqrt(Σ a_i²)
d(a,b) = ||a-b||₂
```

Tests:

- normal values;
- negative and fractional values;
- dimension mismatch;
- zero-length vectors, according to the accepted contract;
- aliasing behavior;
- floating-point tolerance;
- large-magnitude behavior for norm and dot product.

### 1.5 Geometry

- [ ] Normalize.
- [ ] Angle between vectors.
- [ ] Projection.
- [ ] Orthogonality test with tolerance.

Numerical topics:

- division by a zero norm;
- clamping cosine before `acos`;
- overflow/underflow in naive norm computation;
- stable scaled norm as a later improvement.

### Phase 1 exit gate

- Every vector operation has a clear aliasing and error contract.
- Tests distinguish exact integer-like results from approximate floating-point
  results.
- The user can derive dot product, norm, distance, and projection.
- The module passes warning, unit-test, and memory-check builds.

## Phase 2 — Dense matrix core

### 2.1 Representation

Planned baseline:

```c
typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} Matrix;
```

Topics:

- row-major layout;
- mapping `(row, col)` to one-dimensional storage;
- overflow-safe `rows * cols * sizeof(double)`;
- empty-matrix representation;
- ownership and shallow-copy risk.

Deliverables:

- [ ] Lifecycle and tests.
- [ ] Bounded get/set.
- [ ] Zero and identity construction.
- [ ] Fill and deep copy.
- [ ] Transpose into a separate destination.
- [ ] Optional in-place transpose only for justified cases.

### 2.2 Arithmetic

- [ ] Addition and subtraction.
- [ ] Scalar multiplication.
- [ ] Matrix-vector multiplication.
- [ ] Matrix-matrix multiplication.

Dimension contracts:

```text
A(m×n) + B(m×n)
A(m×n) x(n)
A(m×k) B(k×n) = C(m×n)
```

### 2.3 Multiplication performance lab

Implement mathematically equivalent loop orders separately:

- `i-j-k`;
- `i-k-j`;
- at least one intentionally poor access pattern;
- later, simple blocking/tiling.

Measure:

- sizes such as 64, 128, 256, 512, and larger when practical;
- elapsed monotonic time;
- compiler optimization level;
- checksum or correctness comparison;
- repeated runs and median, not one noisy sample.

Explain:

- all versions are `O(n³)`;
- locality changes constants dramatically;
- row-major access favors contiguous traversal;
- benchmarking debug `-O0` and optimized `-O2/-O3` answers different questions.

### Phase 2 exit gate

- Matrix ownership and layout can be drawn from memory.
- Multiplication is tested against small hand-computed examples.
- Benchmark results are reproducible and accompanied by an explanation.

## Phase 3 — Linear systems and elimination

Main problem:

```text
A x = b
```

### 3.1 Triangular systems

- [ ] Forward substitution.
- [ ] Back substitution.
- [ ] Singular/near-zero diagonal policy.

### 3.2 Gaussian elimination

- [ ] Naive elimination for learning.
- [ ] Partial pivoting.
- [ ] Row swap support.
- [ ] Solve without explicitly computing an inverse.

Topics:

- elementary row operations;
- echelon form;
- pivot selection;
- singular and nearly singular matrices;
- growth and rounding error;
- approximate residual `||Ax-b||`.

### 3.3 Verification

- known systems with exact solutions;
- systems requiring a row swap;
- singular systems;
- badly scaled systems;
- residual-based verification;
- comparison of naive versus pivoted elimination.

### Phase 3 exit gate

The user can explain why partial pivoting matters and why checking a residual is
stronger than only comparing against one expected vector.

## Phase 4 — LU, determinant, and inverse

### 4.1 LU factorization

- [ ] `PA = LU` with partial pivoting.
- [ ] Permutation representation.
- [ ] Solve multiple right-hand sides from one factorization.
- [ ] Reconstruction test `PA ≈ LU`.

### 4.2 Determinant

- [ ] Compute from LU diagonal and permutation parity.
- [ ] Explain why cofactor expansion is unsuitable for general computation.

### 4.3 Inverse

- [ ] Implement as a learning exercise using repeated solves.
- [ ] Test `A A⁻¹ ≈ I`.
- [ ] Document why solving `Ax=b` is normally preferable.

### Phase 4 exit gate

The user can compare cost, reuse, and stability of elimination, LU, and explicit
inverse computation.

## Phase 5 — EmtVector spaces, rank, and orthogonality

### Mathematics

- linear combination;
- span;
- linear independence;
- basis and dimension;
- rank and nullity;
- orthogonal and orthonormal sets;
- projection.

### Deliverables

- [ ] Rank from echelon form with a documented tolerance policy.
- [ ] Independence test for a set of vectors.
- [ ] Projection onto one vector.
- [ ] Projection onto an orthonormal basis.
- [ ] Examples showing dependent vectors such as `(1,2,3)` and `(2,4,6)`.

### Numerical warning

Numerical rank is tolerance-dependent. Do not present floating-point rank as an
absolute mathematical truth without documenting the scale and threshold.

## Phase 6 — QR decomposition and least squares

### 6.1 Gram–Schmidt

- [ ] Classical Gram–Schmidt.
- [ ] Modified Gram–Schmidt.
- [ ] Compare loss of orthogonality.
- [ ] Test `QᵀQ ≈ I` and `QR ≈ A`.

### 6.2 Householder QR

- [ ] Understand reflections.
- [ ] Implement after Gram–Schmidt comparison.
- [ ] Document stability and performance differences.

### 6.3 Least squares

- [ ] Fit `y = ax + b`.
- [ ] Implement normal equations for learning.
- [ ] Solve through QR.
- [ ] Compare results on an ill-conditioned example.
- [ ] Report residual and fitted parameters.

### Phase 6 exit gate

The user can explain why normal equations square the condition number and why
QR is preferred for robust least squares.

## Phase 7 — Eigenvalues and eigenvectors

### Foundations

```text
A v = λ v
```

- [ ] Solve selected 2×2 examples by hand.
- [ ] Power iteration for a dominant eigenpair.
- [ ] Convergence tolerance and maximum iteration count.
- [ ] Residual `||Av-λv||`.
- [ ] Failure/slow convergence examples.

### Later algorithms

- [ ] Rayleigh quotient.
- [ ] Inverse iteration after linear solves are mature.
- [ ] Shifted inverse iteration.
- [ ] QR iteration for small dense matrices.
- [ ] Symmetric-matrix specialization.

### Exit gate

Every returned eigenpair is accompanied by a residual, not merely an iteration
count.

## Phase 8 — SVD and low-rank approximation

This is a long-term milestone, not the next feature after vectors.

### Learning path

- [ ] Singular values and geometric interpretation.
- [ ] Relationship between singular values and eigenvalues of `AᵀA`.
- [ ] Educational small-matrix prototype.
- [ ] Recognize the conditioning limitations of forming `AᵀA`.
- [ ] Later study bidiagonalization and a more robust SVD algorithm.

### Image compression demo

Use a simple standard-library-friendly image format such as PGM initially.

- [ ] Parse grayscale PGM.
- [ ] Treat pixels as a matrix.
- [ ] Reconstruct rank-`k` approximations.
- [ ] Compare `k = 5, 20, 50`, subject to image dimensions.
- [ ] Report storage estimate, reconstruction error, and visual result.

Do not claim a production-quality SVD if the implementation only uses an
educational `AᵀA` route.

## Phase 9 — Numerical computing

### 9.1 Root finding

- [ ] Bisection.
- [ ] Newton's method.
- [ ] Secant method.
- [ ] Compare convergence assumptions and failure modes.

### 9.2 Numerical integration

- [ ] Rectangle and trapezoidal rules.
- [ ] Simpson's rule.
- [ ] Step refinement and observed error.

### 9.3 ODE solvers

- [ ] Explicit Euler.
- [ ] Midpoint/RK2.
- [ ] RK4.
- [ ] Generic state derivative callback design.
- [ ] Convergence experiment against a known analytical solution.

### 9.4 Optimization

- [ ] One-dimensional search.
- [ ] Gradient descent on a quadratic.
- [ ] Step-size effects.
- [ ] Later connect least squares and optimization.

### Exit gate

Each method includes assumptions, termination conditions, an error experiment,
and at least one failure example.

## Phase 10 — Particle simulation

Start with a small 2D simulation before a general physics engine.

### Foundations

```text
F = ma
a = F/m
v(t+dt) = v(t) + a dt
x(t+dt) = x(t) + v dt
```

- [ ] 2D particle state.
- [ ] Force accumulation.
- [ ] Gravity and drag.
- [ ] Explicit and semi-implicit Euler.
- [ ] RK methods where appropriate.
- [ ] Energy and trajectory diagnostics.

### Experiments

- projectile motion versus analytical solution;
- harmonic oscillator and energy drift;
- timestep sensitivity;
- integrator comparison.

## Phase 11 — Physics engine foundations

### Math types

- [ ] Fixed-size `Vec2`/`Vec3` types separate from dynamic `EmtVector`.
- [ ] Fixed-size matrices where justified.
- [ ] Rotation matrices.
- [ ] Quaternion representation and normalization.
- [ ] Explain gimbal lock and when quaternions help.

Do not force the dynamic numerical `EmtVector` type into every real-time physics
calculation; fixed-size types have different performance and API needs.

### Collision

- [ ] Circle/sphere overlap.
- [ ] AABB overlap.
- [ ] Closest-point queries.
- [ ] Separating Axis Theorem for convex 2D shapes.
- [ ] Contact normal, depth, and contact point.

### Dynamics

- [ ] Rigid-body state.
- [ ] Linear and angular momentum.
- [ ] Impulse-based collision response.
- [ ] Restitution and friction.
- [ ] Positional correction.
- [ ] Broad phase before optimizing narrow phase.

### Constraints

- [ ] Distance constraint.
- [ ] Sequential impulse solver.
- [ ] Iteration count and convergence experiments.

## Phase 12 — Systems and performance track

This track runs alongside the mathematical phases when justified.

- [ ] Debug, sanitizer, release, and benchmark build profiles.
- [ ] Static library and installed public headers.
- [ ] Dependency files so header changes rebuild correctly.
- [ ] Stable monotonic benchmark timer.
- [ ] Profiling with Linux tools.
- [ ] Cache and branch experiments.
- [ ] Alignment and SIMD only after scalar correctness and measurement.
- [ ] Threading only for operations large enough to amortize overhead.
- [ ] Deterministic tests and reproducible benchmark metadata.
- [ ] CI after the local workflow is stable.

Concurrency is not automatically an improvement. Introduce it only with a
measured workload, a clear data partition, and tests for determinism/races.

## Portfolio milestones

Useful public milestones, each with a focused README/demo:

1. EmtVector library with ownership and tests.
2. Matrix multiplication locality benchmark.
3. Pivoted linear-system solver with residual analysis.
4. QR/least-squares comparison on ill-conditioned data.
5. Eigenvalue convergence visualization.
6. SVD/PGM low-rank image compression.
7. ODE integrator error comparison.
8. Particle simulator with energy plots.
9. Small 2D rigid-body demo.

The “wow” factor should come from explanations, measurements, failure analysis,
and reliable implementation—not merely the number of algorithms.

