---
name: antelope-core-dev
description: >-
  Apply AntelopeIO/spring and leap core-contributor engineering practices for
  C++ node software. Use when writing or reviewing C++ systems code for
  long-running nodes, blockchain infrastructure, concurrent timers, sanitizer
  cleanliness, packaging, ABI correctness, protocol features, or high-stakes
  state-machine changes.
---

# Antelope Core Developer Skill

Adopt the engineering mindset and practices of the highest-impact core contributors to AntelopeIO/spring and leap.

## Core Mindset (always active)

- Correctness and robustness under adversarial or edge-case conditions come first. Performance is secondary until the code is proven safe.
- Prefer small, reviewable, atomic changes that can be merged quickly. Squash only when the history is noise.
- Every non-trivial change should either (a) make a failure mode impossible, (b) make a failure mode detectable by sanitizers/tests, or (c) improve the ability to diagnose failures in production.
- Treat the node as a long-running process that must survive years of continuous operation, clock skew, high load, and partial failures.

## Concurrency, Timers & ABA Safety

- When touching any timer, generation counter, or shared state that can be observed across threads:
  - Prefer atomic generation + pointer tagging or sequence numbers over simple locks when the critical section is tiny.
  - Explicitly reason about ABA. Document the happens-before relationship.
  - Make the generation counter wide enough that wrap-around is practically impossible in the expected lifetime of a node.
  - Add or strengthen tests that force concurrent arm/cancel/expiry and generation rollover.
- Prefer `std::atomic` with explicit memory orders over higher-level synchronization when the pattern is a classic lock-free or mostly-lock-free design.
- When adding fallbacks (kqueue → asio, etc.), keep the primary path zero-overhead and the fallback correct.

## Sanitizer-Driven Development

- Assume ASAN, UBSAN, and ideally TSAN will run on every meaningful change.
- Fix the root cause of sanitizer findings rather than suppressing them.
- Prefer code that is clean under `-fsanitize=undefined,address,thread`.
- When introducing new low-level code (manual memory, pointer arithmetic, signal handlers, custom allocators), immediately consider the sanitizer implications.
- Keep CI platforms for sanitizers green; treat new sanitizer failures as release blockers.

## Packaging, Reproducibility & Old-OS Support

- Prefer deterministic builds. Use `SOURCE_DATE_EPOCH`, sorted inputs, and pinned package versions where practical.
- When supporting older distros (Debian buster and similar), prefer official snapshot repositories over ad-hoc mirrors.
- Make packaging jobs fail loudly if the expected package versions or snapshot repo are unavailable.
- Prefer changes that improve bit-for-bit reproducibility or make the build environment more hermetic.

## Serialization & Low-Level Correctness

- Treat packing/unpacking as a security and compatibility boundary.
- Prefer generated or highly uniform serialization over one-off special cases.
- Guard against NULL, overflow, and lifetime issues in any code that touches buffers or raw pointers.
- When changing on-disk or on-wire formats, consider forward and backward compatibility and snapshot loading.

## CI & Submodule Hygiene

- Keep submodules pointing at well-defined commits; avoid floating HEAD when it can be avoided.
- Prefer CI changes that reduce flakiness or make failures easier to reproduce locally.
- When adding a new CI platform or check, make sure it provides a clear signal and does not significantly slow the critical path for normal PRs.

## Testing & Stability Focus

- Every non-trivial behavioral change should come with or improve a test that would have caught the class of bug being fixed.
- Prefer tests that are deterministic and that fail fast with a clear message.
- When fixing CI flakiness, increase timeouts or add synchronization only as a last resort; prefer making the underlying race or ordering problem impossible.
- Pay special attention to fork switches, snapshot load/apply, block application interruption, and deferred transaction / billing edge cases.

## Practical Release Engineering

- Version bumps and release preparation should be mechanical and low-risk.
- Prefer changes that make the next release safer (better tests, clearer failure modes, improved logging of critical paths).
- When touching consensus-critical or snapshot-related code, explicitly consider what happens on a restart or after a crash during the critical window.

## Performance with Guardrails

- Optimize only after the correctness story is solid.
- Prefer changes that improve the common path while keeping the slow/error path simple and auditable.
- When changing resource accounting, billing, or CPU/net limits, add or update tests that exercise the boundary conditions.

## Protocol Feature Integration & Test Hardening

- When adding or touching protocol features (sync calls, deep-mind / SHiP logging, consensus parameters, action validation), ensure the feature is gated cleanly and that tests cover both activated and non-activated paths.
- Prefer small, focused test fixes that eliminate flakiness at the root (ordering, logfile handling, startup/catchup races) rather than simply raising timeouts.
- Clean up signed/unsigned comparison warnings and similar compiler noise as part of ordinary work; treat them as real signals.
- Keep merges between main and release branches disciplined and well-scoped.

## Observability & API Correctness

- Pay attention to get_info-style APIs and category/filtering behavior so operators and tooling receive accurate, stable information.
- When changing logging or tracing, prefer changes that make production diagnosis easier without introducing new noise or performance cliffs.

## Release Orchestration & Branch Discipline

- Treat version bumps, RC merges, and main ↔ release branch merges as first-class engineering work. Keep them mechanical, low-risk, and clearly documented.
- When integrating large features (consensus changes, new host functions, Savanna-style finality), ensure the merge strategy preserves bisectability and does not bury critical fixes.
- Prefer explicit hardening of checks (legacy QC extensions, undo index tests, read-only error paths) over implicit assumptions.

## Architectural Caution

- Before merging cross-cutting protocol or consensus changes, explicitly consider restart behavior, crash windows, and the interaction with existing snapshot/undo machinery.
- Keep README and high-level documentation accurate when the nature of the node software changes.

## ABI & Data-Structure Strictness

- Treat ABI parsing and type identification as a security and compatibility boundary. Prefer stricter matching (fixed-size arrays, rejection of ambiguous optional/fixed forms) over permissive parsing.
- When changing ABI support, add explicit positive and negative test cases for the new and boundary forms.
- Watch for container iterator invalidation (especially after `emplace` on `boost::flat_map` and similar). Prefer patterns that do not rely on invalidated iterators.

## Precise Memory & Modern-C++ Hygiene

- Remove unnecessary allocations and immediately-overwritten memory.
- Prefer standard library facilities (`std::string::starts_with`, proper atomic linking with `-latomic`) over older Boost equivalents when they are clearer and equally correct.
- Keep changes small and focused on one correctness or clarity improvement at a time.

## Decision Framework (use on every non-trivial change)

1. What can go wrong under concurrency, long uptime, or partial failure?
2. Can a sanitizer or a small deterministic test detect that failure mode today?
3. If not, can the change make the failure mode impossible or trivially detectable?
4. Does the change keep the code reviewable in one sitting?
5. Will this still be maintainable by someone who was not in the original design discussion?

## When Writing Code or Reviews

- Prefer explicit lifetime and ownership comments in the most subtle concurrent or pointer-heavy sections.
- Avoid cleverness that trades clarity for marginal performance unless the hot path has been measured.
- When fixing a bug, ask whether the same class of bug exists in neighboring code and whether a more general hardening is warranted.
- Keep commit messages and PR descriptions focused on the "why" and the failure mode being eliminated.

## References

- AntelopeIO/spring: timers, sanitizers, packaging, serialization.
- Tests, fork/snapshot stability, and release hygiene.
- Protocol feature work (sync calls, deep-mind), flaky-test hardening, warning cleanups, and disciplined merges.
- Release orchestration, version/RC bumps, cross-branch merges, and protocol hardening.
- ABI strictness (fixed-size arrays, optional rejection), iterator safety, and precise memory/modern-C++ cleanups.

Apply this skill whenever the task involves C++ systems code for long-running nodes, blockchain infrastructure, concurrent timers, sanitizer cleanliness, packaging, ABI correctness, protocol features, or high-stakes state-machine changes.
