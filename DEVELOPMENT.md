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

## 0.3.21 — Restore previous detailed power solver
Revert only the primary and screen solver changes from 0.3.19 after reported clicks/pops and loss of audio. Preserve preamp, gain taper, headroom and Economy changes. Original low-level distortion remains unresolved. Build and audio verification pending.

## 0.3.22 — Diagnostic-only baseline
Standalone power_diagnostics measures normal process() path, PI-only and detailed resistive bench across Recto Vintage/Modern, CPU modes, feedback, sample rates and signal levels. CSV reports RMS, peak, fundamental, THD, residual THD+N, supply, screen, bias and elapsed time. Nonfinite or missing output fails; distortion is recorded without speculative pass limits. No DSP changes.

## 0.3.23 — Economy power stage
Economy normal path solves feedback implicitly using identical prior PI, presence and load states per trial. Linear output gain derived approximately from nominal watts and idle bias, with nominal load divider compensation. Retains physical monitoring scale, reactive load and native no-feedback modes. Standard/Max unchanged. CPU cost and convergence/audio unverified; rated power mapping provisional.

## 0.3.24 — Ultra-Lead and native depth controls
Fryette G100UL manual confirms parallel input triodes, optional third lead stage, KT88/6550, 120W and depth. Service bias document specifies 48–52mA. KT88 anchor is provisional (not curve fitted); parallel stage approximated via equivalent resistances; Hi Gain off bypasses third rather than fourth stage. Depth implemented as reduced-order 120Hz feedback shelf (not verified component values), default zero. Peavey/VHT/SLO and EVH Stealth/EL34 only; original EVH 100W no knob. SLO assumes modern depth-equipped variant. 12AT7 driver and GEQ pending. Sources: https://manualzz.com/doc/23615017/fryette-pittbull-ultra-lead-guitar-amplifier-owner-s-manual ; https://forum.fryette.com/uploads/short-url/quxe8dDdZgKM8jnZBXngH3EkODH.pdf . Build/audio unverified.

## 0.3.25 — Pittbull EL34 configuration
Manual EL34 selection retains Pittbull topology and depth, uses provisional 35mA idle target subject to existing dissipation cap and 100W Economy rating instead of 120W. CLX is native EL34 reference, not an Ultra-Lead factory EL34 switch. Exact CLX transformer, rail, driver and bias values remain unverified; shared circuit retained. No graphic EQ. Sources: https://www.fryette.com/support-faqs/ ; https://www.fryette.com/content/G100UL_SFD.pdf . Build/audio unverified.

## 0.3.26 — Detailed feedback stabilization
Uploaded 252-row diagnostic: Economy stable and open-loop gain within 0.32dB; Standard/Max Vintage silent output 55.216V. Solve instantaneous feedback with rollback of PI, coupling, supply and reactive states per trial in both detailed engines. Increase primary solve precision. Expand diagnostics to all ten power models, all CPU modes, representative presence/depth/supply settings and valve overrides; exclude above-Nyquist harmonics, longer settling, flag silence and low-level distortion. Diagnostic failure remains visible but permits VST3 build for audition. Not compiled or measured yet; CPU and convergence risks remain. No valve curve changes.

## 0.3.27 — Build correction
Rename detailed feedback solver local x to feedbackEstimate in Standard and Max to resolve MSVC C2082. No DSP behavior changes. Build pending.


## 0.3.28 — Shared regression correction
Compare resolved and family Rectifier models with identical explicit voice and controls; extend exact equivalence to all twelve voices. Print failing shared combination without changing thresholds. No DSP changes; Marshall stability remains unresolved. Build pending.


## 0.3.29 — Experimental Marshall stability test 1
Standard/Max Marshall power model feeds preamp from a 5 Hz first-order decoupled supply state rather than instantaneous output-stage B+. This is a provisional RC-equivalent isolation experiment, not a measured circuit fit; slow sag retained, no valve or feedback values changed. Economy uses the same routing but its supply is static. Add full/power-only, feedback on/off and Economy/Standard isolation measurements to stability test; original pass criteria unchanged. Build and efficacy unverified. AC30 unresolved.


## 0.3.30 — Independent modular preamp supply (experimental)
Replace 0.3.29 Marshall-only filtered B+ with independent nominal preamp-family rail in Standard/Economy/Max, including preamp-only. Native family nominal B+ times existing 0.65/0.90 factors; provisional anchors, not measured per-stage rails. Power model, rectifier and spongy controls no longer alter preamp voltage; power-stage supply and PI remain unchanged. Add exact preamp-only invariance regression for all eleven preamp enum values. No shared sag in preamp; future linked supply network requires validation. Build and stability unverified.


## 0.3.31 — Shared supply controls, independent rails
Apply spongy (0.85 native B+) and tube rectifier (20 V upstream drop) to each preamp's native nominal supply before existing family scaling. Power stage retains independent target and dynamic sag. Test all four switch combinations for eleven preamp models, exact output independence across power models, and lower preamp rail with either switch. Values remain provisional existing approximations; no shared dynamic sag. Build pending.


## 0.3.32 — Experimental Max linked supply
Max full amp linked mode filters upstream power-stage droop through provisional 10k/22uF RC (220ms), backward Euler, once per sample outside feedback trials; scales native preamp rail by filtered fractional droop. Unlinked, power-only, preamp-only and Economy/Standard remain independent. Add Max linked/unlinked Marshall stability and sag measurements using existing thresholds. Not a full per-node supply network; RC provisional, no guarantee of oscillation elimination; build pending.
