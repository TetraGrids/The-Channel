# Roundtables

Implementation notes from Channel roundtables. Each meeting is **one markdown file** in **exactly one** stage folder.

## For you (how to run this)

1. Drop a transcript or rough notes into [`being-discussed/`](being-discussed/) using the filename `YYYY-MM-DD-short-kebab-topic.md` (meeting date, not today).
2. Ask Cursor to **finalize** the meeting: it should turn the notes into ordered Cursor prompts (and move the file).
3. Work from [`being-implemented/`](being-implemented/). Open **one prompt at a time**, in the order listed in that file. Paste the fenced prompt into a new Cursor chat (or `@` the file and say “do prompt N”).
4. When that meeting’s work is done, **move** the same file to [`completed/`](completed/). Do not copy. Do not leave duplicates.

Load these when coding from a prompt:

- `@antelope-dev-style` — `ra.*` contract C++
- `@antelope-core-dev` — `channeld` / runtime / feature flags
- `@cmake-contract-builds` — new contract source files
- `@never-push` — do not push commits

Details and naming: [`.cursor/skills/roundtables/SKILL.md`](../.cursor/skills/roundtables/SKILL.md).

## Stages

| Folder | Meaning |
| --- | --- |
| [`being-discussed/`](being-discussed/) | Still talking. Transcript, questions, no prompt set yet. |
| [`being-implemented/`](being-implemented/) | Finalized prompts. This is the queue. |
| [`completed/`](completed/) | Done. Same filename, moved here. |

## Current queue

| Meeting | Stage |
| --- | --- |
| [2026-09-09-evm-key-onboarding.md](being-implemented/2026-09-09-evm-key-onboarding.md) | being implemented |
