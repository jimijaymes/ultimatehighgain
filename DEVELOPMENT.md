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
