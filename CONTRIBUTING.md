# Contributing to The Channel

Thanks for contributing. This is the workflow for **this** repository: Spring-family Antelope node software (`channeld`, `chan`, `keyd`, `channel-util`), CDT Wasm contracts (`ra.*`, `flex.token`), CMake, and Make.

Build, install, and test commands live in [README.md](./README.md). Do not duplicate a second setup story here.

## Table of contents

- [Code of conduct](#code-of-conduct)
- [Getting started](#getting-started)
- [Development setup](#development-setup)
- [Branching and commits](#branching-and-commits)
- [Issues](#issues)
- [Pull requests](#pull-requests)
- [Review and merging](#review-and-merging)
- [Testing and CI](#testing-and-ci)
- [Code style](#code-style)
- [Contracts and CMake](#contracts-and-cmake)
- [Documentation](#documentation)
- [Security](#security)
- [Releases](#releases)
- [License](#license)
- [Help](#help)

## Code of conduct

Be respectful, patient, and constructive. Consensus-critical code is reviewed for correctness first, not speed of landing. Report abuse to the maintainers via a private GitHub contact or security advisory (see [Security](#security)).

## Getting started

1. Fork [TetraGrids/The-Channel](https://github.com/TetraGrids/The-Channel).
2. Clone with submodules:

   ```bash
   git clone --recursive https://github.com/<your>/The-Channel.git
   cd The-Channel
   git remote add upstream https://github.com/TetraGrids/The-Channel.git
   ```

3. Branch from current `main`: `git checkout -b feature/short-description`.

`main` is the development branch; production should follow [releases](https://github.com/TetraGrids/The-Channel/releases).

## Development setup

Supported host for node builds: **Ubuntu 22.04**. macOS is best-effort and is not the release path.

Prerequisites, pinned Docker build, unpinned `cmake` / `make`, and `ctest` labels are in the README ([Build and Install from Source](./README.md#build-and-install-from-source)).

Contracts additionally need [Antelope CDT](https://github.com/AntelopeIO/cdt) 3.x/4.x and `-DBUILD_CONTRACTS=ON`. Without local CDT, use the **Compile contracts** GitHub Action (Ubuntu 22.04, CDT 4.1.1).

Some `.cpp` files need ~4GB RAM; reduce `-j` if the compiler is killed.

## Branching and commits

- Names: `feature/…`, `fix/…`, `docs/…`, `chore/…`.
- Small, focused commits. Title states **why**; body can add context.
- Conventional Commit prefixes (`feat:`, `fix:`, `docs:`, `test:`, `chore:`) are welcome; they are not a bot-enforced gate.
- Rebase small local work onto `upstream/main` before opening a PR. Maintainers may ask for squash.

Do not commit CDT-generated `.actions.cpp`, `.dispatch.cpp`, or `.desc` files.

## Issues

Search existing issues and PRs first. Include OS, compiler/CDT versions, commands, and logs. For protocol work, say whether the change is node (`libraries/`, `plugins/`, `programs/`), contracts (`contracts/`), or both.

Pre-mainnet protocol work is tracked in [TO-DO-B4-MAINNET.md](./TO-DO-B4-MAINNET.md).

## Pull requests

Before opening:

- Build the piece you changed (node and/or contracts).
- Add or update tests for behavior changes.
- Format C++ you touched (`clang-format` at the repo root).
- If you add or rename contract sources, update CMake so those files actually compile (see [Contracts and CMake](#contracts-and-cmake)).

PR body:

- Summary and rationale
- Linked issue, if any
- How to validate (cmake/ctest/Action)

Suggested checklist:

- [ ] Builds locally (or Compile contracts is green for contract-only PRs)
- [ ] Tests added or updated
- [ ] Format applied to touched C++
- [ ] Docs updated if user-visible
- [ ] New `.cpp` listed in the matching `add_contract` / `add_subdirectory`

## Review and merging

- At least one maintainer approval for most changes. Consensus, crypto, serialization, and resource-limit changes may need extra review.
- CI that applies to the PR must pass.
- Maintainers choose squash vs merge; prefer a linear history of logical commits.

## Testing and CI

Run **from the build directory**:

```bash
ctest -j "$(nproc)" -LE _tests
ctest -j "$(nproc)" -L wasm_spec_tests
ctest -L "nonparallelizable_tests"
```

Details and long-running tests: README [Step 4 - Test](./README.md#step-4---test).

Boost.Test `--run_test=` is the **suite name** from `BOOST_AUTO_TEST_SUITE()`, not the filename. `unit_test` is not the only binary; plugin tests live under `tests/`. Python TestHarness scripts must run from the **build** directory.

WASM runtime flags: `-- --eos-vm` (and jit/oc if that runtime was built).

GitHub Actions in this repo include **Compile contracts** (path-filtered on `contracts/**`). Other workflows may be Spring leftovers or self-hosted and may not run on this org. Do not assume a green node matrix unless you see it on the PR.

Keep tests deterministic. Do not regenerate snapshot / deep-mind fixtures unless those tests actually consume the saved files and the deployed WASM in the fixture changed.

## Code style

- Node C++: repo-root [`.clang-format`](./.clang-format) (LLVM-based, 3-space indent, 120 columns, pointer left).
- Contracts: CDT C++ (`ACTION` / `TABLE` and existing `ra.*` patterns), not a different Wasm language. There is no Rust or AssemblyScript contract toolchain here.
- CMake: match the nearest sibling `CMakeLists.txt`. Prefer existing `add_contract` / targets. Do not add new options, toolchains, or a second contracts pipeline.
- Python scripts: match neighboring files; there is no required black/flake8 gate unless a workflow adds one.

Consensus paths: no floating point, no uninitialized reads, no undefined behavior, no platform-dependent on-chain results. Prefer named `constexpr` over magic literals; closed sets use enums (`FC_REFLECT` / `FC_REFLECT_ENUM`).

## Contracts and CMake

Root cmake with `-DBUILD_CONTRACTS=ON` builds `contracts/` via the CDT Wasm toolchain.

- New contract directory: `add_subdirectory` in `contracts/CMakeLists.txt` plus a `CMakeLists.txt` copied from the closest similar contract (`ra.claim`-style `src/*.cpp`, or `ra.pure` if sources sit next to CMakeLists).
- New `.cpp` on an existing contract: append it to that directory’s `add_contract(...)`. Headers that are only included do not go on `add_contract`.
- If **Compile contracts** `test -f`s specific `.wasm` paths, add one line for a **new** wasm target only.

## Documentation

Keep [README.md](./README.md), [docs/](./docs/), and contract notes accurate. User-visible behavior (CLI, RAM, keys, resources) needs a doc update in the same PR when practical. Prefer small doc PRs over rewrites.

Roundtable meetings (transcripts → Cursor prompts) live in [roundtables/](./roundtables/). Naming and one-folder-only stages: [`.cursor/skills/roundtables/SKILL.md`](./.cursor/skills/roundtables/SKILL.md).

## Security

Do **not** file public issues for vulnerabilities.

Use [GitHub private vulnerability reporting](https://github.com/TetraGrids/The-Channel/security/advisories/new) when enabled, or contact Tetra Grids maintainers privately. Include impact, affected versions, and a reproduction.

## Releases

Tags are `vMAJOR.MINOR.PATCH` (optional `-rc.N`). Notes go on [GitHub Releases](https://github.com/TetraGrids/The-Channel/releases). Ubuntu 22.04 is the supported release host; do not ship a Mac-only `channeld` as the official binary.

## License

Contributions are under the project [LICENSE](./LICENSE) (MIT). Third-party libraries keep their own licenses in-tree. There is no separate CLA.

## Help

- [GitHub Issues](https://github.com/TetraGrids/The-Channel/issues)
- [README.md](./README.md) for build and test
- [docs/](./docs/) for `channeld` / `chan` / `keyd`
- [WHITEPAPER.md](./WHITEPAPER.md) and [PURE-LIQUID.md](./PURE-LIQUID.md) for protocol intent
