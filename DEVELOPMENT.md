# Development ledger

Read this first on resuming work; verify relevant source before edits. Update this ledger and CHANGELOG.md with each change. Bump CMake project version for releases; retain VST3 ID for session compatibility unless explicitly requested otherwise. Never mark tests passed without results.

## Current status: 0.2.0 development
- Cabinet tab and persistent library: committed, unverified. Rename/delete and basic audio validation committed (06d5691); remaining: state synchronization, invalid import cleanup and host tests.
- Reactive load: fixed Suhr-inspired approximation, no controls; committed, stability and response tests pending. Not a measured Suhr curve.
- Rectifier: early two-channel, Multi-Watt, Road King selectors exist. Rev F reference: user-provided boogie_dualrectifier.PDF, six pages, June 1993 drawing; verify exact switching, presence, feedback, tone stack and supply values against original before changing. 47 nF PI coupling committed. Remaining circuit refinements and calibration pending.
- Build status: not verified for 0.2.0.

## 0.3.0 development
- Shared RP-1F high-gain stage anchors committed (91630deb); approximate interstage networks, not complete nodal transcription. Public reference https://schematicsonline.com/wp-content/uploads/2022/12/Mesa_Boogie_dualrectifier.pdf and https://bench.swellamps.com/preamp-coldclip-1993/ (secondary analysis). Original user file not recovered in this turn.
- Road King channel presence distinction documented by https://mesa-boogie.imgix.net/media/User%20Manuals/Road%20King.pdf; exact pot and feedback wiring remains to verify. Multi-Watt full schematic not verified; manual https://mesa-boogie.imgix.net/media/User%20Manuals/3chRecto_multiWatt.pdf.
- Economy solver iteration budgets reduced (836eb244, 087885f7); no benchmark yet. Standard optimization still pending.
- Outstanding: exact bias, supply, transformer and feedback topology, per-stage supply nodes, complete switching and raw/clean if scoped later; tests and host validation. Existing cold-stage plate pole condition also affects other 39k/100k stages; review isolation before release.

## 0.3.1 development
- Standard solver reuses final screen current and converged branch results (2186c534 and subsequent screen reuse commit). No altered solver tolerances, iterations or circuit values. Economy remains the 0.3.0 reference. Shared reactive history expression reused without changing arithmetic.
- Pending: build, sample-by-sample comparison against 0.3.0 and CPU benchmark. Do not claim measured speedup or bit identity before testing.

## 0.3.2 development
- Both modes now reuse identical redundant evaluations. Economy retains 12 primary, 3 screen and 14 outer PI iterations; Standard retains 16, 6 and 20. No further Economy accuracy reductions until user listening tests. Build, null and CPU benchmarks pending.

## 0.3.3 — Max reference and invariant coefficients
- Max uses separate frozen 0.3.2 Standard DSP headers in reference032; never optimise these headers. Only selected engine runs. Switching to/from Max resets destination state; compare fresh renders, not live switching transients.
- Standard and Economy hoist invariant preamp solver factors without changing dynamic update rates or iteration limits. Existing CPU choice indices retained; host automation normalization compatibility needs testing.
- Build, null tests and CPU measurements pending. Reference is frozen for optimisation comparisons; future circuit changes require an explicit reference policy.

## Max reference policy (supersedes frozen-reference policy above)
- Max is a living maximum-fidelity reference, initially based on 0.3.2 Standard. Apply circuit accuracy improvements and higher-cost fidelity features to Max; do not apply computational simplifications, coefficient-hoisting optimisations, reduced update rates or solver precision reductions to Max.
- Standard and Economy receive shared fidelity-preserving optimisations; retain existing Economy accuracy settings until user testing. For expensive fidelity improvements, implement Max first and evaluate equivalent efficient implementations for Standard/Economy; document any fidelity differences explicitly.
- Keep reference headers independent and record reference revisions so comparisons remain reproducible. No DSP changes in this policy update.

## 0.3.4 — Power amp invariant reuse
- Standard/Economy cache reflected impedance, nominal screen drop, cathode integration denominator and PI specification. Reuse screen collection factors within each branch solve, PI square roots and grid coupling RHS. Dynamic states and solver limits unchanged; reference032 untouched. Build/null/CPU verification pending.

## 0.3.5 — Dependency-specific power amp caches
- Standard/Economy split model, sample interval, rectifier, spongy, presence and tube caches; cache PI DC filter, tail ratio, grid coupling and presence coefficients. Preserve dynamic states, solver limits and existing bias initialization semantics. Max reference headers unchanged. Compilation, null tests and CPU benchmarks pending; not an exhaustive profiling pass.
