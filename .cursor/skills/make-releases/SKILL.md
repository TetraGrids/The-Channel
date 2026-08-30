---
name: make-releases
description: >-
  Cut GitHub releases for TetraGrids/The-Channel. Use when the user asks to
  release, tag, publish, or ship The Channel (channeld, contracts wasm, notes).
---

# Make releases

Repo: `TetraGrids/The-Channel`. Default branch `main` is development; do not treat it as production.

## Preconditions

- Ubuntu 22.04 is the supported build host. Do not release a Mac-only `channeld` binary.
- Contracts: green **Compile contracts** workflow (CDT 4.1.1) or an Ubuntu CDT build of `contracts/`.
- BSL `LICENSE` stays in the tree.
- Follow [never-push-commits](../never-push-commits/SKILL.md): never `git push`. Leave tags local; the user pushes.

## Version

Tags are `vMAJOR.MINOR.PATCH` (optional `-rc.N`). Match `CMakeLists.txt` / version files if this repo bumps them in the same change.

## Cut a release

1. `git status` / `git log` — confirm what lands.
2. Confirm **Compile contracts** is green on the commit you are tagging.
3. If node packages are required, wait for or run the Ubuntu package job from `.github/workflows/` (self-hosted jobs may not run on this org).
4. Create the tag and GitHub release with `gh`:

```bash
git tag -a vX.Y.Z -m "The Channel vX.Y.Z"
# User pushes the tag. Do not git push.
# After they push: gh release create vX.Y.Z --title "vX.Y.Z" --notes-file - <<'EOF'
## Summary
- 

## Contracts
Wasm/ABI from Actions artifact contracts-wasm on this tag (or attach files).

## Node
Ubuntu 22.04 packages when available. macOS is unsupported for production.
EOF
```

5. Attach `contracts-wasm` (and `.deb` if built) with `gh release upload`.
6. Existing `.github/workflows/release.yaml` runs on **published** GitHub releases (experimental binaries image). Publishing the release is what triggers it.

## Notes body

- Why this version exists (fixes, protocol, contracts).
- Call out `flex.token` / RAM / Savanna items that are still on [TO-DO-B4-MAINNET.md](../../../TO-DO-B4-MAINNET.md).
- Digests for protocol features if the boot sequence changed.

## Do not

- Release from a dirty tree.
- Tag `main` as production if [TO-DO-B4-MAINNET.md](../../../TO-DO-B4-MAINNET.md) still has open protocol-permission work you consider a blocker.
- `git push` of commits or tags (the user does that).
