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

The project infrastructure and first vector milestone are in place. EmtVector
lifecycle management currently includes overflow-safe allocation, explicit
zero initialization, destruction, empty-state restoration, and lifecycle
tests. Bounded element access and mathematical vector operations are next.

The matrix module has not been started yet.

## Documentation

- [Technical roadmap](docs/ROADMAP.md)
- [Offline coding plan](docs/OFFLINE_PLAN.md)
- [Architecture decisions](docs/DECISIONS.md)
- [Current project state](docs/ai/PROJECT_STATE.md)
- [Codex–Claude communication protocol](docs/ai/README.md)

## Planned Scope

The project will grow incrementally through the following areas:

1. Vectors and matrices
2. Linear systems and partial pivoting
3. LU and QR decompositions
4. EmtVector spaces, rank, and orthogonality
5. Least-squares problems
6. Eigenvalue algorithms
7. Singular value decomposition and low-rank approximation
8. Numerical integration, root finding, and ODE solvers
9. Particle and rigid-body simulation

## Building

Build the development executable with:

```sh
make
```

Run the executable with:

```sh
make run
```

Build and run every test executable with:

```sh
make test
```

Remove generated build artifacts with:

```sh
make clean
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
