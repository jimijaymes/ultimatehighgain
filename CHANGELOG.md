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

## 0.3.10 — Economy solver optimization
Reuse final screen emission and plate exponential in Standard/Economy; hoist per-sample grid terms. Economy PI tail solver uses bracketed secant with midpoint fallback, unchanged iteration cap and residual tolerance. Cutoff current early return. Max untouched. No new reduced-rate states or precision cuts. Numerical equivalence, runtime and build not verified.

## 0.3.11 — Transformer companion circuit
Implicit magnetizing branch included in primary residual and state update, avoiding explicit stiff feedback through reflected load. Speaker terminal voltage excludes transformer leakage voltage; reactive load preserved. No master taper, tube fit or precision changes. Unverified build/audio; low-master spitting/root cause not yet confirmed.

## 0.3.12 — Diagnostic Economy power amp
Economy full/power-only uses a linear voltage source with model-dependent damping, low-frequency rolloff, presence and passive reactive speaker load. Bypasses nonlinear PI/output tubes, sag and transformer solver; PI-only and direct bench retain existing solver. Standard/Max unchanged. Cabinet switch now applies in every signal mode. Reset on CPU transitions. Frequency response and gain provisional, build unverified.

## 0.3.13 — Economy retains nonlinear phase inverter
Move Economy bypass after PI and model-specific PI voicing, before grid coupling. Retain PI clipping and master drive. Linear downstream reactive load uses provisional differential PI scaling (0.1); explicit global feedback disabled in Economy, represented by damping approximation instead. Standard/Max and cabinet routing unchanged. Build and gain calibration pending.

## 0.3.14 — Gain staging diagnostics
Remove artificial hard digital output clamp in both engines (host floating-point output retains headroom; output trim remains). Preamp-only uses same oversampling interpolation and nominal rectifier/spongy supply settings as full amp. No interstage attenuation or clipping threshold changes. Full amp preamp voltage may legitimately exceed direct input; actual PI sensitivity and output references remain uncalibrated. Standard/Max low-level fault unresolved. Build pending.

## 0.3.15 — Physical voltage headroom
Remove artificial +/-30 V PI input clamp in both engines; retain physical valve/load equations and input calibration. Apply -12 dB monitoring scaling only at each modeled signal output, including Economy; 100 W sine reference now -12 dBFS peak. Raw dry mode unchanged. Diagnostic reference voltages remain provisional. No change to feedback voltage or internal gain. Existing sessions may require output trim adjustment. Build/audio and low-level stability unverified.

## 0.3.16 — Input/output diagnostic trims
Input trim and Output use -24 to +24 dB, default 0 dB, double-click reset. Input before all modes, output after cabinet. Existing Output ID and normalized automation mapping retained; migrate old saved raw output values to dB. No changes to circuit engines. Block-rate gain changes; build/UI verification pending.

## 0.3.17 — Rectifier presence routing and Pittbull switches
Modern presence tone-stack branch enabled for early Orange Modern as provisional 25k range; power-only Modern uses existing PI-side HF contour instead of skipping presence. Economy avoids duplicate Modern presence shelf. Vintage feedback circuit unchanged. Pittbull CL-inspired Hi Gain (3/4 stages), Boost (interstage attenuation), Edge (peaker), Shift (tone-stack caps) added; values provisional, not traced from schematic. Hi Gain defaults on to preserve previous stage count. No KT88, depth or graphic EQ changes. Compile/audio/UI unverified.

## 0.3.18 — Economy closed-loop feedback
Restore native presence/negative feedback network in Economy using previous internal sample's physical speaker terminal voltage. Use provisional open-loop source impedance 1.1 times nominal load, removing duplicated closed-loop damping and generic presence shelf. Native no-feedback models and explicit No Feedback remain open-loop; PI-only remains isolated. Standard/Max unchanged. Loop gain and stability require measurement; build pending.

## 0.3.19 — Low-level solver resolution and gain taper
Detailed primary solve uses safeguarded secant/bisection with 1e-7 V convergence and expandable bracket; screen solve tightened to 1e-7 V residual, up to 32 iterations. No artificial gate or gain reduction. Preamp gain pots use provisional 10% audio taper at midpoint; maximum gain and interstage values unchanged. Exact hardware tapers and interstage measurements pending. Increased CPU cost possible; build and listening tests pending.

## 0.3.20 — Initial schematic-informed gain staging
Early Rectifier gain network uses provisional lumped 2.2M series + 470k stopper + pot resistance and adds 1nF to existing effective peaker; this is not a full nodal transcription (shunt, switched bypass, separate bright cap and 20pF still pending). Rectifier preamp supply ratio 0.90 instead of 0.65 as provisional common-node approximation to 415V at nominal 460V. Stock Marshall audio bass/master taper, approximate 10% midpoint. Input/output calibration unchanged. No power solver changes. Build/audio unverified.
