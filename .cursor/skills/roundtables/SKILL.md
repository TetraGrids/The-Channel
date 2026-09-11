---
name: roundtables
description: >-
  Roundtable meeting files in roundtables/: names, one-folder-only
  lifecycle (being-discussed, being-implemented, completed), and the
  finalized prompt format. Use when adding, moving, or finalizing
  meetings, transcripts, or implementation prompts.
---

# Roundtables

Meetings live under repo-root [`roundtables/`](../../../roundtables/). Human workflow: [`roundtables/README.md`](../../../roundtables/README.md).

## File names

One meeting = one markdown file:

```text
YYYY-MM-DD-short-kebab-topic.md
```

- Date is the **meeting day** (not the day you filed the notes).
- Topic is lowercase kebab-case, no spaces, no participant names unless they are the topic.
- Example: `2026-09-09-evm-key-onboarding.md`

Do not rename when moving folders. Keep the same filename for the life of the meeting.

## One folder only

A meeting file exists in **exactly one** of:

| Folder | When |
| --- | --- |
| `roundtables/being-discussed/` | Notes, transcript, open questions. Not prompt-ready. |
| `roundtables/being-implemented/` | Finalized: ordered Cursor prompts / implementation steps. Work is in progress or ready to start. |
| `roundtables/completed/` | Implementation for that meeting is done. |

**Move** (`git mv`). Never copy. Never leave the same meeting in two stages.

## Finalize format (`being-implemented`)

When promoting from discussed → implemented, rewrite (or append) so the file is **implementation-ready**:

1. Title, date, people.
2. Goal in a few sentences.
3. **Suggested workflow order** (numbered).
4. **Exact Cursor prompts** — one fenced prompt per step, with the skills to load (`@antelope-dev-style`, `@antelope-core-dev`, `@cmake-contract-builds` as needed).
5. Pointers to repo docs (`FUTURE.md`, contracts) and a **gap check**: read the tree before re-implementing work that already landed.
6. Target / test plan if the meeting named one.

Raw transcript stays only in `being-discussed`. After finalize, the file in `being-implemented` is the prompts, not a dump of the call.

## Agent rules

- `@never-push` / `@never-push-commits` still apply. Do not `git push`.
- Do not implement a roundtable in the same turn as filing it unless the user asked to start coding.
- When the user says a meeting is done, `git mv` it to `completed/`.
