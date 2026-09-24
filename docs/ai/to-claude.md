# Request to Claude

Date: 2026-09-24
Status: awaiting review

## Request

Inspect the real repository, then review the vector lifecycle milestone without
changing code.

Please answer:

1. Is the caller-owned `Vector` with a self-owned heap buffer a coherent base
   for this educational library?
2. Is requiring an empty `{0}` object before `initVector` a reasonable explicit
   contract? Identify actual undefined-behavior risks separately from misuse.
3. Are meaningful lifecycle tests missing?
4. Review the global C names `Vector`, `VectorStatus`, `OK`,
   `ERROR_NULL_PTR`, `initVector`, and `destroyVector`. Separate collision risks
   from mere style preference. Do not rename them.
5. Should bounded get/set be the next learning milestone? If not, recommend one
   smaller alternative and justify it.

Write the result to `docs/ai/to-codex.md`. Separate actual bugs, risks,
preferences, and user decisions.

