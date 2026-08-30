#!/usr/bin/env python3
"""Sync TO-DO-B4-MAINNET.md rows with GitHub issues labeled mainnet-todo."""

from __future__ import annotations

import json
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TODO = ROOT / "TO-DO-B4-MAINNET.md"
LABEL = "mainnet-todo"
MARKER = "<!-- to-do-b4-mainnet:{id} -->"
ROW_RE = re.compile(
    r"^\| (?P<id>[^|]+?) \| (?P<task>[^|]+?) \| (?P<issue>[^|]*?) \|\s*$"
)


def gh(*args: str, check: bool = True) -> subprocess.CompletedProcess:
    env = os.environ.copy()
    return subprocess.run(
        ["gh", *args],
        check=check,
        capture_output=True,
        text=True,
        env=env,
        cwd=str(ROOT),
    )


def parse_table(text: str) -> tuple[list[str], list[dict], list[str]]:
    lines = text.splitlines(keepends=True)
    rows: list[dict] = []
    start = end = None
    for i, line in enumerate(lines):
        if line.startswith("| ID |"):
            start = i
            continue
        if start is not None and i == start + 1 and re.match(r"^\|[\s\-:]+\|", line):
            continue
        if start is not None and i >= start + 2:
            m = ROW_RE.match(line.rstrip("\n"))
            if not m:
                end = i
                break
            rows.append(
                {
                    "id": m.group("id").strip(),
                    "task": m.group("task").strip(),
                    "issue": m.group("issue").strip(),
                    "line_index": i,
                }
            )
    if start is None:
        raise SystemExit("TO-DO-B4-MAINNET.md is missing the ID table header")
    if end is None:
        end = len(lines)
    return lines, rows, lines[end:]


def issue_number(cell: str) -> str | None:
    m = re.search(r"/issues/(\d+)", cell)
    if m:
        return m.group(1)
    m = re.search(r"#(\d+)", cell)
    return m.group(1) if m else None


def ensure_label() -> None:
    r = gh("label", "list", "--json", "name", check=False)
    names = []
    if r.returncode == 0 and r.stdout.strip():
        names = [x["name"] for x in json.loads(r.stdout)]
    if LABEL not in names:
        gh("label", "create", LABEL, "--description", "Pre-mainnet todo from TO-DO-B4-MAINNET.md", "--force")


def create_missing_issues() -> bool:
    text = TODO.read_text()
    lines, rows, _ = parse_table(text)
    ensure_label()
    changed = False
    for row in rows:
        if issue_number(row["issue"]):
            continue
        body = (
            f"{MARKER.format(id=row['id'])}\n\n"
            f"{row['task']}\n\n"
            f"Tracked in `TO-DO-B4-MAINNET.md` (ID {row['id']})."
        )
        title = f"[mainnet] {row['task']}"[:240]
        out = gh(
            "issue",
            "create",
            "--title",
            title,
            "--body",
            body,
            "--label",
            LABEL,
        )
        url = out.stdout.strip().splitlines()[-1]
        lines[row["line_index"]] = f"| {row['id']} | {row['task']} | {url} |\n"
        changed = True
        print(f"opened {url} for id {row['id']}")
    if changed:
        TODO.write_text("".join(lines))
    return changed


def remove_closed(number: str) -> bool:
    text = TODO.read_text()
    lines, rows, _ = parse_table(text)
    keep = []
    removed = False
    skip = {r["line_index"] for r in rows if issue_number(r["issue"]) == str(number)}
    if not skip:
        print(f"no table row for issue #{number}")
        return False
    for i, line in enumerate(lines):
        if i in skip:
            removed = True
            continue
        keep.append(line)
    if removed:
        TODO.write_text("".join(keep))
        print(f"removed row for issue #{number}")
    return removed


def main() -> None:
    mode = sys.argv[1] if len(sys.argv) > 1 else "create"
    if mode == "create":
        sys.exit(0 if not create_missing_issues() else 0)
    if mode == "close":
        n = sys.argv[2] if len(sys.argv) > 2 else os.environ.get("CLOSED_ISSUE", "")
        if not n:
            raise SystemExit("usage: sync_mainnet_todo.py close <number>")
        remove_closed(n)
        return
    raise SystemExit(f"unknown mode {mode}")


if __name__ == "__main__":
    main()
