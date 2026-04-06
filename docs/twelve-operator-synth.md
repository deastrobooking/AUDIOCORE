# 12-Operator FM Synthesizer — Technical Reference

**File:** `LIVE-SYSTEMS/synths/Digital/TwelveOperatorSynth.h/.cpp`  
**Plugin:** `plugins/TwelveOperatorSynth/`  
**Namespace:** `LiveSystems::Synths::Digital`

---

## Overview

A polyphonic FM synthesizer engine with 12 operators, a freely-configurable modulation matrix,
per-operator ADSR envelopes, optional Vactrol-style optocoupler envelopes, and a master HP→LP
filter chain. Wrapped as a 4-voice polyphonic VST3/AU/Standalone plugin with a full master FX
chain (Chorus → Delay → Reverb → Compressor).

---

## Algorithm Presets

Set via `setAlgorithmPreset(AlgorithmPreset)` or the **Algorithm** combo in the GUI.

| Preset | Value | Description |
|---|---|---|
| `Custom` | 0 | User-defined modulation matrix (no preset applied) |
| `Stacked` | 1 | All 12 operators stacked in a linear FM chain (op N+1 modulates op N) |
| `Parallel` | 2 | All 12 operators running in parallel, summed |
| `DualStack` | 3 | Two independent 6-op FM stacks, summed |
| `TripletFM` | 4 | Four groups of 3 operators (mod→mod→carrier) |
| `FeedbackChain` | 5 | Linear chain with operator 0 feeding back from operator 11 |
| `StarBurst` | 6 | Operator 0 as central carrier; ops 1-11 each modulate op 0 |
| `Layered` | 7 | Alternating carriers and modulators across all 12 ops |

---

## Per-Operator ADSR

Each of the 12 operators has its own `ADSRSettings`:

```cpp
struct ADSRSettings {
    float attackMs  = 10.0f;    // Attack time in milliseconds
    float decayMs   = 100.0f;   // Decay time in milliseconds
    float sustain   = 0.8f;     // Sustain level [0.0 – 1.0]
    float releaseMs = 200.0f;   // Release time in milliseconds
};
```

**API:**

```cpp
// Apply the same ADSR to all 12 operators at once
synth.setGlobalADSR(attackMs, decayMs, sustain, releaseMs);

// Set ADSR for a specific operator (0 – 11)
synth.setOperatorADSR(int operatorIndex, const ADSRSettings& settings);
```

The ADSR is a linear per-sample state machine:

- **Attack** ramps gain from 0 → 1 over `attackMs`
- **Decay** ramps gain from 1 → `sustain` over `decayMs`
- **Sustain** holds at `sustain` level while note is held
- **Release** ramps gain to 0 over `releaseMs`; operator becomes `Idle` when gain reaches 0

---

## Vactrol Envelope

Models an analogue optocoupler (LED + LDR) for non-linear amplitude shaping applied to the
master output of the voice after all operators are mixed.

```cpp
struct VactrolSettings {
    bool  enabled   = false;
    float attackMs  = 5.0f;     // LED turn-on time (fast)
    float releaseMs = 300.0f;   // LDR dark-resistance decay (slow)
    float sag       = 0.3f;     // Sag coefficient — extends release on retrigger [0 – 1]
    float curve     = 0.5f;     // LDR photoresistance curve [0 – 1]; higher = more logarithmic
};
```

**API:**

```cpp
synth.setVactrolSettings(const VactrolSettings& v);
```

**Transfer function:**

```
coeff_attack  = 1 - exp(-2.2 / (attackMs  * sampleRate / 1000))
coeff_release = 1 - exp(-2.2 / ((releaseMs + sagAccum * sag * 3000) * sampleRate / 1000))
ldrOutput     = pow(vactrolValue, 1 + curve * 2)   // nonlinear LDR response
```

`sagAccum` accumulates on each `noteOn()` retrigger, modelling voltage sag in analogue circuitry.

---

## Master Filter Chain (HP → LP)

A second-order state-variable filter pair is applied in series after vactrol processing.

```cpp
synth.setHighPassCutoff(float hz);      // default 20 Hz
synth.setHighPassResonance(float q);    // default 0.707
synth.setLowPassCutoff(float hz);       // default 20000 Hz
synth.setLowPassResonance(float q);     // default 0.707
synth.setFiltersEnabled(bool enabled);  // master enable/bypass
```

Implemented with `juce::dsp::StateVariableTPTFilter` (2-pole TPT).

---

## Voice Lifecycle API

```cpp
synth.noteOn(float velocity);   // triggers ADSR + vactrol, range 0.0–1.0
synth.noteOff();                // moves all active envelopes to Release stage
bool active = synth.isActive(); // true while note is on OR any envelope is non-Idle
```

---

## Plugin Architecture

### Voice Polyphony

```
maxVoices = 4  (configurable via TwelveOperatorSynthProcessor::maxVoices)
```

Voice stealing uses **oldest age** — when all 4 voices are active, the voice that received its
`noteOn` earliest is stolen for the new note.

### Parameter List

| Parameter ID | Range | Description |
|---|---|---|
| `mode` | 0–3 | Synthesis mode combo |
| `algorithm` | 0–7 | Algorithm preset (see table above) |
| `masterGain` | 0.0–2.0 | Output gain before FX chain |
| `analogDrift` | 0.0–0.05 | Per-operator frequency drift amount |
| `attack` | 1–5000 ms | Global ADSR attack |
| `decay` | 1–5000 ms | Global ADSR decay |
| `sustain` | 0.0–1.0 | Global ADSR sustain level |
| `release` | 1–10000 ms | Global ADSR release |
| `vactrolEnable` | toggle | Enable Vactrol envelope |
| `vactrolAttack` | 1–500 ms | Vactrol attack |
| `vactrolRelease` | 10–5000 ms | Vactrol release |
| `vactrolSag` | 0.0–1.0 | Retrigger sag amount |
| `vactrolCurve` | 0.0–1.0 | LDR nonlinearity |
| `hpCutoff` | 10–2000 Hz | High-pass filter cutoff |
| `hpQ` | 0.1–10.0 | High-pass resonance |
| `lpCutoff` | 200–20000 Hz | Low-pass filter cutoff |
| `lpQ` | 0.1–10.0 | Low-pass resonance |
| `chorusEnable` | toggle | Enable chorus |
| `chorusRate` | 0.1–10 Hz | Chorus LFO rate |
| `chorusDepth` | 0.0–1.0 | Chorus depth |
| `chorusMix` | 0.0–1.0 | Chorus wet/dry |
| `delayEnable` | toggle | Enable simple delay |
| `delayTime` | 0.01–2.0 s | Delay time |
| `delayFeedback` | 0.0–0.95 | Delay feedback |
| `delayMix` | 0.0–1.0 | Delay wet/dry |
| `reverbEnable` | toggle | Enable reverb (Freeverb) |
| `reverbSize` | 0.0–1.0 | Room size |
| `reverbDamp` | 0.0–1.0 | Damping |
| `reverbMix` | 0.0–1.0 | Reverb wet/dry |
| `compEnable` | toggle | Enable compressor |
| `compThreshold` | -60–0 dB | Compressor threshold |
| `compRatio` | 1.0–20.0 | Compressor ratio |

### Master FX Chain

Applied post-voice-mix in the `processBlock()`:

```
Σ voices → Chorus → SimpleDelay → Reverb → Compressor → output
```

Each effect can be individually bypassed via its toggle parameter.

---

## Signal Flow

```
MIDI noteOn
    │
    ├─ noteOn(velocity) ──► all 12 operator ADSR stages → Attack
    │                        vactrol sagAccum += existing value
    │
    └─ render(buffer, numSamples)
           │
           ├─ for each operator i:
           │      envGain[i] = tickEnvelope(i)      // ADSR
           │      opSample   = processOperatorSample(i, modInput)
           │      contribution = envGain[i] * opSample
           │
           ├─ mix all operator contributions
           │
           ├─ vactrolGain = tickVactrol()            // vactrol envelope
           │      mixed *= vactrolGain
           │
           ├─ highpassFilter.processSample(ch, val)  // HP
           └─ lowpassFilter.processSample(ch, val)   // LP
```

---

## Build Integration

```cmake
target_link_libraries(MyPlugin
    PRIVATE
        LIVE-SYSTEMS-Digital   # TwelveOperatorSynth engine
        LIVE-SYSTEMS-AudioEffects
        LIVE-SYSTEMS-Framework
)
```

The full plugin is built via:

```bash
cmake --build build --target TwelveOperatorSynth
```
