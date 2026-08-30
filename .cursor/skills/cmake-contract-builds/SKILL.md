---
name: cmake-contract-builds
description: >-
  After contract source changes, verify and apply only the CMake wiring so new
  files are built. Use when adding or renaming contracts, .cpp/.hpp under
  contracts/, add_contract, add_subdirectory, or Compile contracts CI wasm checks.
---

# CMake contract builds

After assessing how this repo actually builds contracts, update CMake (and only the matching CI path check) so **new contract files are compiled**. Nothing else new. Minimum accurate.

## How contracts build here

1. Root `CMakeLists.txt`: `-DBUILD_CONTRACTS=ON` runs ExternalProject on `contracts/` with `CDTWasmToolchain.cmake`. Do not add a second contracts pipeline.
2. `contracts/CMakeLists.txt`: one `add_subdirectory(<dir>)` per contract (or `test_contracts`).
3. Each contract dir has `CMakeLists.txt` using `add_contract(<target> <name> <sources...>)`, then the same `target_include_directories` / `set_target_properties` / ricardian `-R` pattern as its **nearest sibling**, not a new style.
4. Multi-source contracts (`ra.system`): extra `.cpp` files must be listed on `add_contract(...)`. A new `.hpp` is not listed unless it is compiled as its own translation unit (it is not).
5. GitHub **Compile contracts** job cmake's `contracts/` the same way. If that job `test -f` specific `.wasm` paths, add one line for a **new** wasm target only.

## After each contract change

1. List new/renamed/deleted sources under `contracts/` (ignore `libraries/`, `plugins/`, docs).
2. Decide the existing target:
   - New dir → `add_subdirectory` in `contracts/CMakeLists.txt` + a `CMakeLists.txt` copied from the closest similar contract (`ra.claim` for a single `src/*.cpp`, `ra.pure` for sources in the dir root).
   - New `.cpp` on an existing `add_contract` → append that path only.
   - Deleted source → remove from `add_contract` / drop `add_subdirectory` if the dir is gone.
   - Header-only include used by an already-listed `.cpp` → no CMake change.
3. Includes: add a `target_include_directories` entry only if the new sources will not compile without it (same as siblings: `include/`, `../ra.token/include` when they include `ra.token`).
4. Do not add new CMake options, find_package calls, toolchains, README sections, extra contracts, or node/plugin CMake.

## Stop

CMake (and the one wasm `test -f` if needed) is the whole job. Do not “improve” the build.
