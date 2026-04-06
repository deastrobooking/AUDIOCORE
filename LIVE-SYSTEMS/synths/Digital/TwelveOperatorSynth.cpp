#include "TwelveOperatorSynth.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float pi = 3.14159265358979323846f;
constexpr float twoPi = 6.28318530717958647692f;
}

namespace LiveSystems::Synths::Digital
{
void TwelveOperatorSynth::prepare(double newSampleRate)
{
    sampleRate = newSampleRate > 1000.0 ? newSampleRate : 44100.0;
    reset();
}

void TwelveOperatorSynth::reset()
{
    phases.fill(0.0f);
    previousOutputs.fill(0.0f);
    driftState.fill(0.0f);
}

void TwelveOperatorSynth::setMode(Mode newMode)
{
    mode = newMode;
}

void TwelveOperatorSynth::setMasterGain(float gainLinear)
{
    masterGain = juce::jmax(0.0f, gainLinear);
}

void TwelveOperatorSynth::setGlobalAnalogDrift(float amount)
{
    globalAnalogDrift = juce::jlimit(0.0f, 0.2f, amount);
}

void TwelveOperatorSynth::setOperator(int index, const Operator& settings)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)] = settings;
}

const TwelveOperatorSynth::Operator& TwelveOperatorSynth::getOperator(int index) const
{
    jassert(isValidOperatorIndex(index));
    return operators[static_cast<size_t>(juce::jlimit(0, numOperators - 1, index))];
}

void TwelveOperatorSynth::setOperatorLevel(int index, float level)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].level = juce::jlimit(0.0f, 1.0f, level);
}

void TwelveOperatorSynth::setOperatorRatio(int index, float ratio)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].ratio = juce::jmax(0.01f, ratio);
}

void TwelveOperatorSynth::setOperatorDetune(int index, float detuneHz)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].detuneHz = juce::jlimit(-200.0f, 200.0f, detuneHz);
}

void TwelveOperatorSynth::setOperatorPan(int index, float pan)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].pan = juce::jlimit(0.0f, 1.0f, pan);
}

void TwelveOperatorSynth::setOperatorFeedback(int index, float feedback)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].feedback = juce::jlimit(0.0f, 1.0f, feedback);
}

void TwelveOperatorSynth::setOperatorPhaseDistortion(int index, float amount)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].phaseDistortion = juce::jlimit(0.0f, 1.0f, amount);
}

void TwelveOperatorSynth::setOperatorPulseWidth(int index, float pulseWidth)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].pulseWidth = juce::jlimit(0.05f, 0.95f, pulseWidth);
}

void TwelveOperatorSynth::setOperatorWaveform(int index, Waveform waveform)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].waveform = waveform;
}

void TwelveOperatorSynth::setOperatorEnabled(int index, bool enabled)
{
    if (!isValidOperatorIndex(index))
        return;

    operators[static_cast<size_t>(index)].enabled = enabled;
}

void TwelveOperatorSynth::clearModulationMatrix()
{
    for (auto& row : modulationMatrix)
        row.fill(0.0f);
}

void TwelveOperatorSynth::setModulationIndex(int sourceOperator, int destinationOperator, float index)
{
    if (!isValidOperatorIndex(sourceOperator) || !isValidOperatorIndex(destinationOperator))
        return;

    modulationMatrix[static_cast<size_t>(destinationOperator)][static_cast<size_t>(sourceOperator)] = juce::jlimit(-8.0f, 8.0f, index);
}

float TwelveOperatorSynth::getModulationIndex(int sourceOperator, int destinationOperator) const
{
    if (!isValidOperatorIndex(sourceOperator) || !isValidOperatorIndex(destinationOperator))
        return 0.0f;

    return modulationMatrix[static_cast<size_t>(destinationOperator)][static_cast<size_t>(sourceOperator)];
}

void TwelveOperatorSynth::setAlgorithmStacked()
{
    clearModulationMatrix();

    for (int i = 0; i < numOperators - 1; ++i)
        setModulationIndex(i + 1, i, 1.2f);
}

void TwelveOperatorSynth::setAlgorithmParallel()
{
    clearModulationMatrix();

    for (int i = 1; i < numOperators; ++i)
        setModulationIndex(i, 0, 0.35f);
}

void TwelveOperatorSynth::render(juce::AudioBuffer<float>& buffer,
                                 int startSample,
                                 int numSamples,
                                 float fundamentalHz,
                                 float amplitude,
                                 bool addToOutput)
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
            currentOutputs[static_cast<size_t>(op)] = processOperatorSample(op, fundamentalHz, currentOutputs);

        float left = 0.0f;
        float right = 0.0f;

        for (int op = 0; op < numOperators; ++op)
        {
            const auto& settings = operators[static_cast<size_t>(op)];
            if (!settings.enabled)
                continue;

            const float opOut = currentOutputs[static_cast<size_t>(op)] * settings.level;
            const float pan = settings.pan;
            const float leftGain = std::sqrt(1.0f - pan);
            const float rightGain = std::sqrt(pan);

            left += opOut * leftGain;
            right += opOut * rightGain;
        }

        left = juce::jlimit(-1.0f, 1.0f, left * safeAmplitude);
        right = juce::jlimit(-1.0f, 1.0f, right * safeAmplitude);

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

float TwelveOperatorSynth::processOperatorSample(int opIndex,
                                                 float fundamentalHz,
                                                 const std::array<float, numOperators>& currentOutputs)
{
    const auto& settings = operators[static_cast<size_t>(opIndex)];
    if (!settings.enabled)
        return 0.0f;

    float modulation = 0.0f;
    for (int src = 0; src < numOperators; ++src)
        modulation += modulationMatrix[static_cast<size_t>(opIndex)][static_cast<size_t>(src)] * currentOutputs[static_cast<size_t>(src)];

    float driftHz = 0.0f;
    if (mode == Mode::VirtualAnalog && globalAnalogDrift > 0.0f)
    {
        auto& drift = driftState[static_cast<size_t>(opIndex)];
        drift += juce::Random::getSystemRandom().nextFloat() * 0.002f - 0.001f;
        drift = juce::jlimit(-1.0f, 1.0f, drift);
        driftHz = drift * globalAnalogDrift * 5.0f;
    }

    const float operatorFreq = juce::jmax(0.0f, (fundamentalHz * settings.ratio) + settings.detuneHz + driftHz);
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
            const float pdAmount = juce::jlimit(0.0f, 1.0f, settings.phaseDistortion + std::abs(modulation) * 0.15f);
            const float pdPhase = applyPhaseDistortion(wrapPhase(phase + feedbackInput * 0.05f), pdAmount);
            signal = applyWaveform(settings.waveform, pdPhase, settings.pulseWidth);
            break;
        }
        case Mode::VirtualAnalog:
        {
            phase = wrapPhase(phase + phaseStep + modulation * 0.003f);
            signal = applyWaveform(settings.waveform, wrapPhase(phase + feedbackInput * 0.02f), settings.pulseWidth);

            const float subPhase = wrapPhase(phase * 0.5f);
            const float sub = std::sin(twoPi * subPhase) * 0.2f;
            signal = juce::jlimit(-1.0f, 1.0f, signal + sub);
            break;
        }
    }

    phases[static_cast<size_t>(opIndex)] = phase;
    previousOutputs[static_cast<size_t>(opIndex)] = signal;
    return signal;
}

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

float TwelveOperatorSynth::wrapPhase(float phase)
{
    while (phase >= 1.0f)
        phase -= 1.0f;
    while (phase < 0.0f)
        phase += 1.0f;
    return phase;
}
} // namespace LiveSystems::Synths::Digital
