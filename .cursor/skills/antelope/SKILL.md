---
name: antelope
description: >-
  Antelope / Spring / The Channel node and contract engineering. Use when
  building channeld, chan, keyd, channel-util, ra.* contracts, tests, plugins,
  libfc, consensus, or WASM runtimes in this repo.
---

# Antelope / Spring / The Channel

This tree is Spring-family Antelope node software. Binaries are **`channeld`**, **`chan`**, **`keyd`**, **`channel-util`**. Contracts are **`ra.*`** and **`flex.token`**. WASM runtimes are **`eos-vm`**, **`eos-vm-jit`**, **`eos-vm-oc`**.

Contract C++: [antelope-dev-style](../antelope-dev-style/SKILL.md). Node C++: [antelope-core-dev](../antelope-core-dev/SKILL.md). CMake for new contract files: [cmake-contract-builds](../cmake-contract-builds/SKILL.md). Never `git push`: [never-push](../never-push/SKILL.md).

## Code quality

Prefer the best correct solution. Consensus determinism: no floating point in on-chain/consensus paths, no uninitialized reads, no UB, no platform-dependent on-chain results.

- Read the relevant code. No partial “quick fixes” when a complete design is needed.
- Tests for normal, edge, and error paths; assert behavior, not just compile.
- Validate at boundaries. Prefer compile-time checks.

### No duplicated helpers

Same helper in two TUs: extract. Plugin-internal: anonymous namespace or private member. Shared across plugins: owning plugin `include/` or `libraries/libfc/` if no plugin dep. Shared by subclasses: `protected` (or `static` if stateless) on the base.

### No magic literals

Named `constexpr` for strings and numbers that are not trivial indexes. Contract account / table / action / index names: nested `namespace` of `constexpr` identifiers.

### Enums over raw values

Closed sets use the enum. Prefer `FC_REFLECT_ENUM`. Decode variants as the enum type, not `static_cast` of a uint.

## Build

Follow this repo’s `README.md`. Typical:

```bash
git submodule update --init --recursive
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j $(nproc)
```

Some `.cpp` files need ~4GB RAM; drop `-j` if the compiler is killed. macOS `channeld` is best-effort, not the release path.

Contracts: `-DBUILD_CONTRACTS=ON` (CDT 4.1.x) or the **Compile contracts** GitHub Action.

## Test

From the **build directory**:

```bash
ctest -j "$(nproc)" -LE _tests
ctest -j "$(nproc)" -L wasm_spec_tests
ctest -L "nonparallelizable_tests"
ctest -L "long_running_tests"
```

Log long runs: `ctest ... --output-on-failure --timeout 1000 2>&1 | tee /tmp/ctest-run.log`.

Boost.Test `--run_test=` is the **suite name** from `BOOST_AUTO_TEST_SUITE()`, not the filename. `unit_test` does not contain every test; also `plugin_test` / `tests/` and libfc tests if those targets exist. If `no test cases matching filter`, try the other binary.

Python TestHarness tests: run **from the build directory** (`python3 tests/<test>.py` there), not the source root.

WASM runtime flags: `-- --eos-vm` (and jit/oc if built).

Regenerate deep-mind / snapshot fixtures only when those tests use save flags and the fixture’s deployed WASM actually changed. Delete **both** source and build snapshot files first; copy build artifacts to source only if tests require committed fixtures. Do not regenerate snapshots because an unrelated contract rebuilt.

## Layout

- `libraries/` — libfc, chain, appbase, testing
- `plugins/` — linked into `channeld` (`plugins/usage_pattern.md` if present)
- `programs/` — `channeld`, `chan`, `keyd`, `channel-util`
- `contracts/` — `ra.system`, `ra.token`, `flex.token` (`ra.pure/`), …
- `unittests/`, `tests/`

Serialization: `FC_REFLECT` / `FC_REFLECT_ENUM`, `fc::variant`.

CDT may emit `.actions.cpp` / `.dispatch.cpp` / `.desc` — do not commit; delete if they appear untracked.

## Style

Repo `.clang-format` if present (Spring-family node C++ often uses 3-space indent, 120 columns, pointer left). Contract files follow antelope-dev-style (`ACTION` / `TABLE`), not node style.

## Releases

[make-releases](../make-releases/SKILL.md). Do not ship from an unverified Mac node build.
