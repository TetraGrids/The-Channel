---
name: never-push
description: >-
  Never git push for the user. Use on every git, commit, tag, or GitHub
  operation in The Channel. Don't ever push commits for me.
---

# Never push

**Don't ever push commits for me.**

Do not run `git push`, `git push --tags`, `git push origin`, `git push -u`, or any equivalent (`gh` that publishes a branch, `git send-pack`).

Allowed: local `git add`, `git commit` when the user asked for a commit, `git status`, `git diff`, `git log`, `git tag` **locally** if they asked for a tag.

If a release or remote update is needed, stop after the local commit or local tag and tell the user the exact commands to run themselves.
