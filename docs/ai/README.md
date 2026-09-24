# Codex–Claude Communication Protocol

This directory provides asynchronous, repository-based communication between
Codex and Claude Code. The repository and its tests remain the source of truth;
handoff files are review messages, not authority.

## Files

| File | Owner | Purpose |
|---|---|---|
| `PROJECT_STATE.md` | Either assistant, after verification | Current canonical project state |
| `to-claude.md` | Codex | The current question or review request for Claude |
| `to-codex.md` | Claude | Claude's response for Codex |
| `../DECISIONS.md` | User-approved updates only | Durable architecture decisions |

## Codex to Claude

1. Codex inspects the current repository and tests.
2. Codex updates `PROJECT_STATE.md` only if the recorded state is stale.
3. Codex replaces the request section in `to-claude.md`.
4. The user opens Claude Code in the repository root and sends:

```text
CLAUDE.md, docs/ai/PROJECT_STATE.md, docs/DECISIONS.md ve
docs/ai/to-claude.md dosyalarını oku. Gerçek repo dosyalarını incele.
Kodda değişiklik yapmadan değerlendirmeyi docs/ai/to-codex.md dosyasına yaz.
```

## Claude to Codex

1. Claude inspects the repository.
2. Claude writes its review to `to-codex.md`.
3. The user tells Codex:

```text
Claude docs/ai/to-codex.md dosyasına cevap yazdı. Gerçek kodla karşılaştır,
katıldığın ve katılmadığın noktaları açıkla, sonra sıradaki tek görevi ver.
```

4. Codex independently verifies every correctness claim.
5. The user decides on architecture or naming changes.
6. Accepted decisions are added to `docs/DECISIONS.md`.

## Rules that prevent confusion

- Only Codex writes `to-claude.md`; only Claude writes `to-codex.md`.
- Messages are replaced for each review; Git preserves history.
- Neither assistant treats the other assistant's opinion as fact.
- `PROJECT_STATE.md` records what exists, not speculative plans.
- `ROADMAP.md` records plans; `DECISIONS.md` records accepted choices.
- Source code and passing tests override stale documentation.

