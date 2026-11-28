# Development ledger

Read this first on resuming work; verify relevant source before edits. Update this ledger and CHANGELOG.md with each change. Bump CMake project version for releases; retain VST3 ID for session compatibility unless explicitly requested otherwise. Never mark tests passed without results.

## Current status: 0.2.0 development
- Cabinet tab and persistent library: committed, unverified. Remaining: rename/delete, validation, state synchronization, host tests.
- Reactive load: fixed Suhr-inspired approximation, no controls; committed, stability and response tests pending. Not a measured Suhr curve.
- Rectifier: early two-channel, Multi-Watt, Road King selectors exist. Rev F reference: user-provided boogie_dualrectifier.PDF, six pages, June 1993 drawing; verify exact switching, presence, feedback, tone stack and supply values against original before changing. 47 nF PI coupling committed. Remaining circuit refinements and calibration pending.
- Build status: not verified for 0.2.0.
