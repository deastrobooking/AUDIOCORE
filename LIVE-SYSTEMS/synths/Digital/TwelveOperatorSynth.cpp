#include "TwelveOperatorSynth.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float pi    = 3.14159265358979323846f;
constexpr float twoPi = 6.28318530717958647692f;
}

namespace LiveSystems::Synths::Digital
{
// ============================================================================
// Lifecycle
// ============================================================================
void TwelveOperatorSynth::prepare(double newSampleRate)
{
    sampleRate = newSampleRate > 1000.0 ? newSampleRate : 44100.0;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = 4096;
    spec.numChannels      = 2;

    highpassFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highpassFilter.setCutoffFrequency(hpCutoff);
    highpassFilter.setResonance(hpResonance);
    highpassFilter.prepare(spec);

    lowpassFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lowpassFilter.setCutoffFrequency(lpCutoff);
    lowpassFilter.setResonance(lpResonance);
    lowpassFilter.prepare(spec);

    reset();
}

void TwelveOperatorSynth::reset()
{
    phases.fill(0.0f);
    previousOutputs.fill(0.0f);
    driftState.fill(0.0f);

    for (auto& env : envelopeStates)
    {
        env.stage    = EnvelopeStage::Idle;
        env.value    = 0.0f;
        env.velocity = 1.0f;
    }

    vactrolState = {};

    highpassFilter.reset();
    lowpassFilter.reset();

    noteIsOn = false;
}

// ============================================================================
// Note control
// ============================================================================
void TwelveOperatorSynth::noteOn(float velocity)
{
    const float vel = juce::jlimit(0.0f, 1.0f, velocity);
    noteIsOn = true;

    if (vactrol.enabled && vactrolState.value > 0.01f)
        vactrolState.sagAccum = juce::jlimit(0.0f, 1.0f, vactrolState.sagAccum + 0.2f);
    else
        vactrolState.sagAccum *= 0.8f;

    for (int op = 0; op < numOperators; ++op)
    {
        auto& state    = envelopeStates[static_cast<size_t>(op)];
        state.stage    = EnvelopeStage::Attack;
        state.velocity = vel;
    }
}

void TwelveOperatorSynth::noteOff()
{
    noteIsOn = false;

    for (int op = 0; op < numOperators; ++op)
    {
        auto& state = envelopeStates[static_cast<size_t>(op)];
        if (state.stage != EnvelopeStage::Idle)
            state.stage = EnvelopeStage::Release;
    }
}

bool TwelveOperatorSynth::isActive() const noexcept
{
    if (noteIsOn)
        return true;

    for (const auto& env : envelopeStates)
        if (env.stage != EnvelopeStage::Idle)
            return true;

    return false;
}

// ============================================================================
// Mode / gain / drift
// ============================================================================
void TwelveOperatorSynth::setMode(Mode newMode) { mode = newMode; }

void TwelveOperatorSynth::setMasterGain(float gainLinear)
{
    masterGain = juce::jmax(0.0f, gainLinear);
}

void TwelveOperatorSynth::setGlobalAnalogDrift(float amount)
{
    globalAnalogDrift = juce::jlimit(0.0f, 0.2f, amount);
}

// ============================================================================
// Per-operator
// ============================================================================
void TwelveOperatorSynth::setOperator(int index, const Operator& settings)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)] = settings;
}

const TwelveOperatorSynth::Operator& TwelveOperatorSynth::getOperator(int index) const
{
    jassert(isValidOperatorIndex(index));
    return operators[static_cast<size_t>(juce::jlimit(0, numOperators - 1, index))];
}

void TwelveOperatorSynth::setOperatorLevel(int index, float level)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].level = juce::jlimit(0.0f, 1.0f, level);
}

void TwelveOperatorSynth::setOperatorRatio(int index, float ratio)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].ratio = juce::jmax(0.01f, ratio);
}

void TwelveOperatorSynth::setOperatorDetune(int index, float detuneHz)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].detuneHz = juce::jlimit(-200.0f, 200.0f, detuneHz);
}

void TwelveOperatorSynth::setOperatorPan(int index, float pan)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].pan = juce::jlimit(0.0f, 1.0f, pan);
}

void TwelveOperatorSynth::setOperatorFeedback(int index, float feedback)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].feedback = juce::jlimit(0.0f, 1.0f, feedback);
}

void TwelveOperatorSynth::setOperatorPhaseDistortion(int index, float amount)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].phaseDistortion = juce::jlimit(0.0f, 1.0f, amount);
}

void TwelveOperatorSynth::setOperatorPulseWidth(int index, float pulseWidth)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].pulseWidth = juce::jlimit(0.05f, 0.95f, pulseWidth);
}

void TwelveOperatorSynth::setOperatorSubLevel(int index, float subLevel)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].subLevel = juce::jlimit(0.0f, 1.0f, subLevel);
}

void TwelveOperatorSynth::setOperatorWaveform(int index, Waveform waveform)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].waveform = waveform;
}

void TwelveOperatorSynth::setOperatorEnabled(int index, bool enabled)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].enabled = enabled;
}

void TwelveOperatorSynth::setOperatorADSR(int index, const ADSRSettings& adsr)
{
    if (!isValidOperatorIndex(index)) return;
    operators[static_cast<size_t>(index)].adsr = adsr;
}

void TwelveOperatorSynth::setGlobalADSR(const ADSRSettings& adsr)
{
    for (auto& op : operators)
        op.adsr = adsr;
}

// ============================================================================
// Vactrol
// ============================================================================
void TwelveOperatorSynth::setVactrolSettings(const VactrolSettings& settings)
{
    vactrol = settings;
}

// ============================================================================
// Master filter chain
// ============================================================================
void TwelveOperatorSynth::setHighPassCutoff(float frequencyHz)
{
    hpCutoff = juce::jlimit(10.0f, 20000.0f, frequencyHz);
    highpassFilter.setCutoffFrequency(hpCutoff);
}

void TwelveOperatorSynth::setHighPassResonance(float q)
{
    hpResonance = juce::jlimit(0.1f, 8.0f, q);
    highpassFilter.setResonance(hpResonance);
}

void TwelveOperatorSynth::setLowPassCutoff(float frequencyHz)
{
    lpCutoff = juce::jlimit(20.0f, 20000.0f, frequencyHz);
    lowpassFilter.setCutoffFrequency(lpCutoff);
}

void TwelveOperatorSynth::setLowPassResonance(float q)
{
    lpResonance = juce::jlimit(0.1f, 8.0f, q);
    lowpassFilter.setResonance(lpResonance);
}

void TwelveOperatorSynth::setFiltersEnabled(bool enabled)
{
    filtersEnabled = enabled;
    if (!enabled)
    {
        highpassFilter.reset();
        lowpassFilter.reset();
    }
}

// ============================================================================
// Modulation matrix
// ============================================================================
void TwelveOperatorSynth::clearModulationMatrix()
{
    for (auto& row : modulationMatrix)
        row.fill(0.0f);
}

void TwelveOperatorSynth::setModulationIndex(int sourceOperator, int destinationOperator, float index)
{
    if (!isValidOperatorIndex(sourceOperator) || !isValidOperatorIndex(destinationOperator))
        return;

    modulationMatrix[static_cast<size_t>(destinationOperator)][static_cast<size_t>(sourceOperator)]
        = juce::jlimit(-8.0f, 8.0f, index);
}

float TwelveOperatorSynth::getModulationIndex(int sourceOperator, int destinationOperator) const
{
    if (!isValidOperatorIndex(sourceOperator) || !isValidOperatorIndex(destinationOperator))
        return 0.0f;

    return modulationMatrix[static_cast<size_t>(destinationOperator)][static_cast<size_t>(sourceOperator)];
}

// ============================================================================
// Algorithm presets
// ============================================================================
void TwelveOperatorSynth::setAlgorithmPreset(AlgorithmPreset preset)
{
    currentAlgorithm = preset;
    clearModulationMatrix();

    switch (preset)
    {
        case AlgorithmPreset::Custom:
            break;

        case AlgorithmPreset::Stacked:
            for (int i = 0; i < numOperators - 1; ++i)
                setModulationIndex(i + 1, i, 1.2f);
            break;

        case AlgorithmPreset::Parallel:
            for (int i = 1; i < numOperators; ++i)
                setModulationIndex(i, 0, 0.35f);
            break;

        case AlgorithmPreset::DualStack:
            for (int i = 0; i < 5; ++i)  setModulationIndex(i + 1, i, 1.2f);
            for (int i = 6; i < 11; ++i) setModulationIndex(i + 1, i, 1.2f);
            break;

        case AlgorithmPreset::TripletFM:
            for (int g = 0; g < 3; ++g)
            {
                const int base = g * 4;
                for (int i = 0; i < 3; ++i)
                    setModulationIndex(base + i + 1, base + i, 1.1f);
            }
            break;

        case AlgorithmPreset::FeedbackChain:
            for (int i = 0; i < numOperators - 1; ++i)
                setModulationIndex(i + 1, i, 1.0f);
            for (int i = 0; i < numOperators; ++i)
                operators[static_cast<size_t>(i)].feedback
                    = juce::jlimit(0.0f, 1.0f, operators[static_cast<size_t>(i)].feedback + 0.4f);
            break;

        case AlgorithmPreset::StarBurst:
            for (int i = 0; i < numOperators - 1; ++i)
                setModulationIndex(numOperators - 1, i, 0.5f);
            break;

        case AlgorithmPreset::Layered:
            for (int i = 0; i < 6; ++i)
                setModulationIndex(i + 6, i, 1.0f);
            break;
    }
}

// ============================================================================
// Render
// ============================================================================
void TwelveOperatorSynth::render(juce::AudioBuffer<float>& buffer,
                                 int   startSample,
                                 int   numSamples,
                                 float fundamentalHz,
                                 float amplitude,
                                 bool  addToOutput)
{
    if (numSamples <= 0 || fundamentalHz <= 0.0f)
        return;

    const int numChannels = buffer.getNumChannels();
    if (numChannels <= 0)
        return;

    const float safeAmplitude = juce::jlimit(0.0f, 1.0f, amplitude) * masterGain;

    if (!addToOutput)
        buffer.clear(startSample, numSamples);

    std::array<float, numOperators> currentOutputs {};

    for (int sample = 0; sample < numSamples; ++sample)
    {
        currentOutputs.fill(0.0f);

        for (int op = 0; op < numOperators; ++op)
        {
            const float envGain = tickEnvelope(op);
            currentOutputs[static_cast<size_t>(op)]
                = processOperatorSample(op, fundamentalHz, currentOutputs) * envGain;
        }

        const float vactrolGain = tickVactrol();

        float left  = 0.0f;
        float right = 0.0f;

        for (int op = 0; op < numOperators; ++op)
        {
            const auto& settings = operators[static_cast<size_t>(op)];
            if (!settings.enabled)
                continue;

            const float opOut     = currentOutputs[static_cast<size_t>(op)] * settings.level;
            const float leftGain  = std::sqrt(1.0f - settings.pan);
            const float rightGain = std::sqrt(settings.pan);

            left  += opOut * leftGain;
            right += opOut * rightGain;
        }

        left  *= safeAmplitude * vactrolGain;
        right *= safeAmplitude * vactrolGain;

        // Master filter chain: HP then LP, per-sample stereo
        if (filtersEnabled)
        {
            left  = lowpassFilter.processSample(0, highpassFilter.processSample(0, left));
            right = lowpassFilter.processSample(1, highpassFilter.processSample(1, right));
        }

        left  = juce::jlimit(-1.0f, 1.0f, left);
        right = juce::jlimit(-1.0f, 1.0f, right);

        const int bufferSample = startSample + sample;
        if (numChannels == 1)
        {
            const float mono = 0.5f * (left + right);
            buffer.setSample(0, bufferSample, buffer.getSample(0, bufferSample) + mono);
        }
        else
        {
            buffer.setSample(0, bufferSample, buffer.getSample(0, bufferSample) + left);
            buffer.setSample(1, bufferSample, buffer.getSample(1, bufferSample) + right);

            for (int ch = 2; ch < numChannels; ++ch)
            {
                const float mono = 0.5f * (left + right);
                buffer.setSample(ch, bufferSample, buffer.getSample(ch, bufferSample) + mono);
            }
        }
    }
}

// ============================================================================
// Per-sample envelope tick
// ============================================================================
float TwelveOperatorSynth::tickEnvelope(int opIndex)
{
    auto& state          = envelopeStates[static_cast<size_t>(opIndex)];
    const auto& settings = operators[static_cast<size_t>(opIndex)].adsr;
    const float sr       = static_cast<float>(sampleRate);

    switch (state.stage)
    {
        case EnvelopeStage::Idle:
            return 0.0f;

        case EnvelopeStage::Attack:
        {
            const float rate = 1.0f / juce::jmax(1.0f, settings.attackMs * 0.001f * sr);
            state.value += rate;
            if (state.value >= 1.0f)
            {
                state.value = 1.0f;
                state.stage = EnvelopeStage::Decay;
            }
            return state.value * state.velocity;
        }

        case EnvelopeStage::Decay:
        {
            const float range = 1.0f - settings.sustain;
            if (range <= 0.0f)
            {
                state.value = settings.sustain;
                state.stage = EnvelopeStage::Sustain;
                return settings.sustain * state.velocity;
            }
            const float rate = range / juce::jmax(1.0f, settings.decayMs * 0.001f * sr);
            state.value -= rate;
            if (state.value <= settings.sustain)
            {
                state.value = settings.sustain;
                state.stage = EnvelopeStage::Sustain;
            }
            return state.value * state.velocity;
        }

        case EnvelopeStage::Sustain:
            return settings.sustain * state.velocity;

        case EnvelopeStage::Release:
        {
            const float rate = state.value / juce::jmax(1.0f, settings.releaseMs * 0.001f * sr);
            state.value -= rate;
            if (state.value <= 0.0f)
            {
                state.value = 0.0f;
                state.stage = EnvelopeStage::Idle;
            }
            return state.value * state.velocity;
        }
    }
    return 0.0f;
}

// ============================================================================
// Per-sample vactrol tick
// ============================================================================
float TwelveOperatorSynth::tickVactrol()
{
    if (!vactrol.enabled)
        return 1.0f;

    const float sr     = static_cast<float>(sampleRate);
    const float target = noteIsOn ? 1.0f : 0.0f;

    float coeff;
    if (target > vactrolState.value)
    {
        // Fast optical attack
        const float attackSamples = juce::jmax(1.0f, vactrol.attackMs * 0.001f * sr);
        coeff = 1.0f - std::exp(-2.2f / attackSamples);
    }
    else
    {
        // Slow LDR release, extended by sag accumulation
        const float effectiveReleaseMs = vactrol.releaseMs
                                         * (1.0f + vactrolState.sagAccum * vactrol.sag * 3.0f);
        const float releaseSamples = juce::jmax(1.0f, effectiveReleaseMs * 0.001f * sr);
        coeff = 1.0f - std::exp(-2.2f / releaseSamples);
        vactrolState.sagAccum *= (1.0f - 0.01f / releaseSamples);
    }

    vactrolState.value += coeff * (target - vactrolState.value);

    float out = vactrolState.value;
    if (vactrol.curve > 0.0f)
        out = std::pow(juce::jlimit(0.0f, 1.0f, out), 1.0f + vactrol.curve * 2.0f);

    return out;
}

// ============================================================================
// processOperatorSample
// ============================================================================
float TwelveOperatorSynth::processOperatorSample(int opIndex,
                                                 float fundamentalHz,
                                                 const std::array<float, numOperators>& currentOutputs)
{
    const auto& settings = operators[static_cast<size_t>(opIndex)];
    if (!settings.enabled)
        return 0.0f;

    float modulation = 0.0f;
    for (int src = 0; src < numOperators; ++src)
        modulation += modulationMatrix[static_cast<size_t>(opIndex)][static_cast<size_t>(src)]
                      * currentOutputs[static_cast<size_t>(src)];

    float driftHz = 0.0f;
    if (mode == Mode::VirtualAnalog && globalAnalogDrift > 0.0f)
    {
        auto& drift = driftState[static_cast<size_t>(opIndex)];
        drift += rng.nextFloat() * 0.002f - 0.001f;
        drift  = juce::jlimit(-1.0f, 1.0f, drift);
        driftHz = drift * globalAnalogDrift * 5.0f;
    }

    const float operatorFreq = juce::jmax(0.0f,
        (fundamentalHz * settings.ratio) + settings.detuneHz + driftHz);
    const float phaseStep = static_cast<float>(operatorFreq / sampleRate);
    float phase = phases[static_cast<size_t>(opIndex)];
    const float feedbackInput = previousOutputs[static_cast<size_t>(opIndex)] * settings.feedback;

    float signal = 0.0f;
    switch (mode)
    {
        case Mode::FM:
        {
            phase = wrapPhase(phase + phaseStep);
            const float modulatedPhase = wrapPhase(phase + (modulation + feedbackInput) * 0.25f);
            signal = std::sin(twoPi * modulatedPhase);
            break;
        }
        case Mode::PhaseDistortion:
        {
            phase = wrapPhase(phase + phaseStep);
            const float pdAmount = juce::jlimit(0.0f, 1.0f,
                settings.phaseDistortion + std::abs(modulation) * 0.15f);
            const float pdPhase = applyPhaseDistortion(
                wrapPhase(phase + feedbackInput * 0.05f), pdAmount);
            signal = applyWaveform(settings.waveform, pdPhase, settings.pulseWidth);
            break;
        }
        case Mode::VirtualAnalog:
        {
            phase = wrapPhase(phase + phaseStep + modulation * 0.003f);
            signal = applyWaveform(settings.waveform,
                                   wrapPhase(phase + feedbackInput * 0.02f),
                                   settings.pulseWidth);

            // Sub-oscillator at half frequency, level controlled per operator
            if (settings.subLevel > 0.0f)
            {
                const float subPhase = wrapPhase(phase * 0.5f);
                const float sub      = std::sin(twoPi * subPhase) * settings.subLevel;
                signal = juce::jlimit(-1.0f, 1.0f, signal + sub);
            }
            break;
        }
    }

    phases[static_cast<size_t>(opIndex)]          = phase;
    previousOutputs[static_cast<size_t>(opIndex)] = signal;
    return signal;
}

// ============================================================================
// Static helpers
// ============================================================================
bool TwelveOperatorSynth::isValidOperatorIndex(int index) noexcept
{
    return index >= 0 && index < numOperators;
}

float TwelveOperatorSynth::applyWaveform(Waveform waveform, float phase, float pulseWidth)
{
    const float p = wrapPhase(phase);

    switch (waveform)
    {
        case Waveform::Sine:
            return std::sin(twoPi * p);
        case Waveform::Saw:
            return (2.0f * p) - 1.0f;
        case Waveform::Square:
            return p < juce::jlimit(0.05f, 0.95f, pulseWidth) ? 1.0f : -1.0f;
        case Waveform::Triangle:
            return 1.0f - 4.0f * std::abs(p - 0.5f);
    }
    return 0.0f;
}

float TwelveOperatorSynth::applyPhaseDistortion(float phase, float amount)
{
    const float split = juce::jlimit(0.05f, 0.95f, 0.5f + (amount - 0.5f) * 0.8f);
    if (phase < split)
        return 0.5f * (phase / split);

    return 0.5f + 0.5f * ((phase - split) / (1.0f - split));
}

// Use std::fmod for safe wrapping at any phase step size
float TwelveOperatorSynth::wrapPhase(float phase) noexcept
{
    phase = std::fmod(phase, 1.0f);
    return phase < 0.0f ? phase + 1.0f : phase;
}

} // namespace LiveSystems::Synths::Digital
