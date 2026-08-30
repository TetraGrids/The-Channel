---
name: antelope
description: >-
  Antelope / Spring / The Channel node and contract engineering guidance ported
  from Wire Sysio CLAUDE.md. Use when building channeld, chan, keyd,
  channel-util, ra.* contracts, tests, plugins, libfc, consensus, or WASM
  runtimes in this repo.
---

# Antelope (The Channel)

Port of [Wire Sysio `CLAUDE.md`](https://github.com/Wire-Network/wire-sysio/blob/main/CLAUDE.md) onto this tree. Binaries here are **`channeld`**, **`chan`**, **`keyd`**, **`channel-util`**. Contracts are **`ra.*`** and **`flex.token`**. WASM VM is **eos-vm** (not sys-vm). This repo does **not** use Wire OPP, vcpkg, or `nodeop`/`clio`.

Contract C++ style: [antelope-dev-style](../antelope-dev-style/SKILL.md). Node C++: [antelope-core-dev](../antelope-core-dev/SKILL.md).

## Code quality

Prefer the best correct solution. Consensus determinism first: no floating point in consensus paths, no uninitialized reads, no UB, no platform-dependent on-chain results.

- Understand the full problem. No partial “quick fixes” when a complete design is needed.
- Tests for normal, edge, and error paths.
- Validate at boundaries. Prefer compile-time checks.

### No duplicated helpers

Same helper in two TUs: extract. Plugin-internal: anonymous namespace or private member. Shared: owning plugin `include/` or `libraries/libfc/`. Shared by subclasses: `protected` on the base.

### No magic literals

Named `constexpr` for strings and numbers that are not trivial indexes. Contract account / table / action names: nested `namespace` of `constexpr` identifiers.

### Enums over raw values

Closed sets use the enum. Prefer `FC_REFLECT_ENUM`. This tree has no protobuf OPP enums; do not invent Wire `sysio::opp` types.

## Build (Ubuntu 22.04)

Supported OS is Ubuntu 22.04. See repo `README.md`. Contracts: CDT 4.1.x and `-DBUILD_CONTRACTS=ON`, or the **Compile contracts** GitHub Action.

```bash
git submodule update --init --recursive
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j $(nproc)
```

Some `.cpp` files need ~4GB RAM; drop `-j` if the compiler is killed.

macOS is best-effort. Do not treat a Mac `channeld` build as the release path.

## Test

From the build dir:

```bash
ctest -j "$(nproc)" -LE _tests
ctest -j "$(nproc)" -L wasm_spec_tests
ctest -L "nonparallelizable_tests"
```

Log ctest: `ctest ... --output-on-failure 2>&1 | tee /tmp/ctest-run.log`.

Boost.Test `--run_test=` is the **suite name** from `BOOST_AUTO_TEST_SUITE()`, not the filename. `unit_test` does not contain every test; plugin tests live under `tests/`.

Python TestHarness tests must run **from the build directory**.

## Layout

- `libraries/` — libfc, chain, appbase, testing
- `plugins/` — linked into `channeld` (`plugins/usage_pattern.md`)
- `programs/` — `channeld`, `chan`, `keyd`, `channel-util`
- `contracts/` — `ra.system`, `ra.token`, `flex.token` (`ra.pure/`), …
- `unittests/`, `tests/`

Serialization: `FC_REFLECT` / `FC_REFLECT_ENUM`, `fc::variant`.

CDT may emit `.actions.cpp` / `.dispatch.cpp` / `.desc` — do not commit them.

## Style

Repo `.clang-format` if present. Spring-family C++ often uses 3-space indent. Contract files follow antelope-dev-style (`ACTION` / `TABLE`), not node style.

## Releases

Use [make-releases](../make-releases/SKILL.md). Do not ship from an unverified Mac node build.
