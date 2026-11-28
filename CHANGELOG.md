# Changelog

## 0.3.2 — Shared solver optimisation (unverified)
- Apply identical redundant screen-current and converged branch reuse to Economy as Standard.
- Preserve Economy iteration budgets and tolerances; no further approximations.
- Build, null comparison and CPU benchmark pending.

## 0.3.1 — Standard solver redundancy removal (unverified)
- Reuse final screen-current evaluation and converged branch results at identical primary voltage; reuse reactive history.
- Economy iteration budgets and branch evaluation path retained. No circuit or tolerance changes.
- Build, null comparison and CPU benchmark pending.

## 0.3.0 — Rectifier and economy solver development (unverified)
- RP-1F-inspired stage values and cold-stage pole, early Orange Modern presence routing: 91630deb.
- Rectifier feedback seed and economy power/PI iteration budgets: 836eb244, 087885f7.
- Standard solver iteration budgets unchanged; CPU and tonal equivalence not benchmarked.

## 0.2.0 — 2026-11-28 (development, unverified)
- Cabinet tab, persistent multi-file IR import, browser, previous/next, FFT display, embedded IR state: 646fb995ba16666cc29ac309da6f2dd743529762.
- Fixed generic reactive load and reflected load-line coupling; Rectifier 47 nF PI coupling: 86f7d3bcd533f2d65dd86cdee0bd4d490535bc52.
- Existing Rectifier architecture/channel/voicing selectors retained.
- IR rename and confirmed deletion, basic audio format validation: 06d5691f176b2298b1275859823ddeeb5a490d80.
- Build, hardware calibration and host validation pending.

## 0.1.0
- Existing modular amplifier DSP and JUCE plugin baseline.

## 0.3.3 — Max reference and invariant coefficients
- Max uses separate frozen 0.3.2 Standard DSP headers in reference032; never optimise these headers. Only selected engine runs. Switching to/from Max resets destination state; compare fresh renders, not live switching transients.
- Standard and Economy hoist invariant preamp solver factors without changing dynamic update rates or iteration limits. Existing CPU choice indices retained; host automation normalization compatibility needs testing.
- Build, null tests and CPU measurements pending. Reference is frozen for optimisation comparisons; future circuit changes require an explicit reference policy.

## 0.3.4 — Power amp invariant reuse
- Standard/Economy cache reflected impedance, nominal screen drop, cathode integration denominator and PI specification. Reuse screen collection factors within each branch solve, PI square roots and grid coupling RHS. Dynamic states and solver limits unchanged; reference032 untouched. Build/null/CPU verification pending.

## 0.3.5 — Dependency-specific power amp caches
- Standard/Economy split model, sample interval, rectifier, spongy, presence and tube caches; cache PI DC filter, tail ratio, grid coupling and presence coefficients. Preserve dynamic states, solver limits and existing bias initialization semantics. Max reference headers unchanged. Compilation, null tests and CPU benchmarks pending; not an exhaustive profiling pass.
