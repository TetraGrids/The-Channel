---
name: mainnet-todo-issues
description: >-
  Keep TO-DO-B4-MAINNET.md in sync with GitHub issues. Use when adding or
  editing pre-mainnet todos, the mainnet todo table, or when a mainnet-todo
  issue is closed.
---

# Mainnet todo issues

Source of truth: repo-root [`TO-DO-B4-MAINNET.md`](../../../TO-DO-B4-MAINNET.md) (markdown table).

Automation: [`.github/workflows/mainnet-todo-sync.yml`](../../../.github/workflows/mainnet-todo-sync.yml) and [`.github/scripts/sync_mainnet_todo.py`](../../../.github/scripts/sync_mainnet_todo.py).

## When a row is added

1. Append a row: `| <next-id> | <task> | |` (Issue column empty).
2. Do not invent issue URLs by hand unless `gh` already created them.
3. After the file is on `main` (or the default branch), the workflow opens an issue labeled `mainnet-todo` and writes the URL into the Issue column.
4. If the user wants the issue immediately (before push):

```bash
python3 .github/scripts/sync_mainnet_todo.py create
gh issue list --label mainnet-todo
```

Then commit the table if the script filled URLs.

Issue body must keep `<!-- to-do-b4-mainnet:<ID> -->`.

## When an issue is closed

The workflow removes that row from the table. Do not leave a “done” row.

If the workflow cannot push (branch protection), delete the matching table row in a PR and cite the closed issue number.

## Do not

- Close the issue and leave the row.
- Add a todo only as a GitHub issue without a table row (the table is the checklist).
- Reuse an ID after deletion; pick a new unused ID.
