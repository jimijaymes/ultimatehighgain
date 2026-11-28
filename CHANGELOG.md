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

## 0.3.6 — Experimental coupled phase inverter
- Max receives new implicit grid loading via backward-Euler coupling and piecewise grid conduction relative to cathode. Standard/Economy use same equations with safeguarded Newton plate and bracketed secant cathode steps; identical new residual tolerances and iteration ceilings. Existing other solver settings unchanged. Preserves inverted plate-output convention; unloaded DC reference initialized once in loaded path. AC30 cut and standalone Modern shaping remain downstream, not jointly solved; grid load is reduced-order, not a verified hardware fit. Build, convergence, harmonic, null and CPU tests pending. Earlier reference remains available in Git history.

## 0.3.7 — Gain staging diagnostics
Power amp only and PI only bypass preamp; master is independent of preamp-only, uses provisional square taper in full/power paths. Channel volume exposed for non-Marshall models as provisional post-tone attenuation; Jose clipping volume retained. Removed hard clamp in preamp monitor. Temporarily bypass experimental loaded PI in all modes to isolate idle hum; original code retained. Cabinet bypassed in dry/preamp/PI checks. Output reference provisional 100 W into nominal load. Build and audio tests pending; hum not verified resolved.

## 0.3.8 — Calibration bench
Added standalone calibration executable, default Rectifier sweep and --all for all power models. Direct calibrated PI input bypasses master/preamp; internal resistive 8-ohm test option, reactive remains default. CSV reports raw secondary voltage, DC, RMS, resistive watts, harmonics 2–10 THD, supply/bias and runtime. No fitted parameters changed. Resistive branch retains approximate transformer saturation; output voltage and reflected impedance conventions require validation. No build or measurement verified yet. Run cmake --build build --config Release --target calibration, then calibration > calibration.csv (or --all). Further tests for current, bursts, IMD and null comparison pending.

## 0.3.9 — Output stage stability investigation
User reports PI-only works, full/power-only spit or cut out with high idle meter. Removed inconsistent post-solve transformer saturation from both engines; reactive load retained. Implicit reservoir update and screen solver denominator guard. These are diagnostic corrections, not verified fixes. Restore physically coupled transformer saturation after calibration. Build and bench results pending.
