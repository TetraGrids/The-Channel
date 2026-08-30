---
name: contract-cmake-verify
description: >-
  After contract or CMake edits, verify add_subdirectory, add_contract, sources,
  includes, ricardians, and CDT 4.1.1 compile wiring. Use when adding, renaming,
  or moving ra.* / flex.token contracts, CMakeLists, or compile-contracts.yml.
---

# Contract CMake verify

After any contract change, do not stop at the `.cpp`. Assess how that contract is **meant** to build, then confirm CMake and CI match.

## 1. Assess the build

- Target name (`add_contract` first arg) vs on-chain account (`[[eosio::contract("…")]]`). Dots are allowed in the account string; CMake target may match (e.g. `flex.token`).
- Every translation unit listed in `add_contract(...)` — new `.cpp` files are not picked up by glob unless listed.
- Public includes: `target_include_directories` for this contract and sibling headers (`ra.token`, `ra.system`).
- Extra contracts in the same dir (`rex.results`, `powup.results`) each need their own `add_contract`.
- Ricardians: `configure_file` + `target_compile_options(... -R...)` if `.md.in` exists.
- Test contracts: `test_contracts/` and INTERFACE includes from `ra.system` if they wrap it.
- Root: `contracts/CMakeLists.txt` must `add_subdirectory` the new folder.
- Node tree: root `CMakeLists.txt` `BUILD_CONTRACTS` ExternalProject already points at `contracts/`; only change it if toolchain args must change.
- CI: `.github/workflows/compile-contracts.yml` — CDT 4.1.1, `CDTWasmToolchain.cmake`, and `test -f` for any wasm that must exist.

## 2. Proper CMake upgrades (checklist)

For each added/renamed/removed contract:

- [ ] Directory has `CMakeLists.txt`
- [ ] Parent `add_subdirectory(...)`
- [ ] `add_contract(<name> <name> …sources)` lists **all** `.cpp` that belong
- [ ] Include dirs cover headers used by those sources
- [ ] Rename: old `add_contract` / paths / CI `test -f` updated; no stale `grams` targets
- [ ] CI list/upload still finds `build-contracts/<dir>/<name>.wasm`

## 3. After assessing, verify the compile path

Do **not** assume Homebrew `eosio-cpp` 1.8.1. Proper contract build is CDT 3/4 via:

- GitHub Action **Compile contracts**, or
- Ubuntu: cmake `contracts/` with `CDTWasmToolchain.cmake`

If CDT is not on this machine, state that and still complete the CMake checklist; do not call the change verified until that compile path would include the new sources.

## 4. Fail the task

If a new contract file is not in `add_contract` / not in a subdirectory of `contracts/CMakeLists.txt`, the CMake upgrade is incomplete.
