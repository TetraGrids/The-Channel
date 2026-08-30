---
name: never-push-commits
description: >-
  Never git-push commits for the user of The Channel. Use whenever committing,
  tagging, gh, git push, origin, or publishing is considered.
---

# Never push commits

don't ever push commits for me

Never run `git push` (any remote, any branch, any tag) and never use `gh` in a way that publishes commits (`gh pr create` after a push, `gh release create` that uploads from an unpublished tag you just pushed, etc.).

Allowed: local `git commit`, local `git tag` (not pushed), show the user the exact `git push` / `gh` commands.

Not allowed: `git push`, `git push -u`, `git push origin HEAD`, `git push --tags`, force push, or asking the harness to push on the user's behalf.
