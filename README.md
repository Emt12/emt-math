# EMT-MATH

EMT-MATH is a linear algebra and numerical computing library written from
scratch in ISO C99. The project is intended as a long-term study of the path
from mathematical definitions to reliable, testable, and efficient systems
software.

The goal is not merely to collect implementations. Each feature is developed
by first understanding the mathematics, then designing the algorithm and C
API, and finally validating the implementation with tests, numerical error
analysis, and benchmarks.

## Goals

- Build a linear algebra library without using an external linear algebra
  dependency.
- Develop a practical understanding of memory ownership, pointers, data
  layout, and API design in C.
- Connect linear algebra with numerical methods, computer architecture, and
  performance analysis.
- Compare mathematical complexity with real measurements, including cache and
  memory-access effects.
- Provide a foundation for later simulation, robotics, and physics-engine
  projects.

## Current Status

EMT-MATH is in its initial infrastructure phase. The development environment,
repository structure, compiler flags, test workflow, and debugging tools are
currently being established. The public vector and matrix APIs have not been
implemented yet.

## Planned Scope

The project will grow incrementally through the following areas:

1. Vectors and matrices
2. Linear systems and partial pivoting
3. LU and QR decompositions
4. Vector spaces, rank, and orthogonality
5. Least-squares problems
6. Eigenvalue algorithms
7. Singular value decomposition and low-rank approximation
8. Numerical integration, root finding, and ODE solvers
9. Particle and rigid-body simulation

## Building

The project currently uses a manual development build while the Makefile is
being designed:

```sh
mkdir -p build
gcc -std=c99 -Wall -Wextra -Wpedantic -g3 -O0 \
    src/main.c \
    -o build/emt-math
```

Run the executable with:

```sh
./build/emt-math
```

## Project Principles

- Understand the mathematics before implementing the algorithm.
- Keep memory ownership explicit.
- Treat dimension mismatches and allocation failures deliberately.
- Test normal cases, edge cases, and floating-point behavior separately.
- Prefer solving linear systems directly over explicitly computing inverses.
- Document algorithmic complexity and numerical-stability limitations.
- Benchmark only after establishing correctness.

## License

A license will be selected before the first public release.
