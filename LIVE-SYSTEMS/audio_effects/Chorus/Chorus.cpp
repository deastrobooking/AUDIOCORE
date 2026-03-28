#include "Chorus.h"
#include <cmath>

namespace LiveSystems::AudioEffects
{

void Chorus::prepare(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    lfoPhase = 0.0f;

    int maxDelaySamples = static_cast<int>(sampleRate * maxDelayMs / 1000.0);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = 1;

    delayLines.clear();
    for (int ch = 0; ch < 2; ++ch)
    {
        delayLines.emplace_back(maxDelaySamples);
        delayLines.back().prepare(spec);
        delayLines.back().reset();
    }
}

void Chorus::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    if (numChannels == 0 || numSamples == 0) return;

    // Ensure enough delay lines
    while (static_cast<int>(delayLines.size()) < numChannels)
    {
        int maxDelaySamples = static_cast<int>(currentSampleRate * maxDelayMs / 1000.0);
        delayLines.emplace_back(maxDelaySamples);
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = currentSampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(numSamples);
        spec.numChannels = 1;
        delayLines.back().prepare(spec);
    }

    float phaseInc = static_cast<float>(rate / currentSampleRate);

    for (int s = 0; s < numSamples; ++s)
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            // Phase offset between channels for stereo width
            float phaseOffset = (ch == 0) ? 0.0f : 0.25f;
            float lfoVal = getNextLFOValue(phaseOffset);

            // Modulated delay time in samples
            float modulatedDelay = static_cast<float>((centreDelay + lfoVal * depth) * currentSampleRate / 1000.0);
            modulatedDelay = juce::jlimit(1.0f, static_cast<float>(maxDelayMs * currentSampleRate / 1000.0 - 1), modulatedDelay);

            auto& delay = delayLines[static_cast<size_t>(ch)];
            delay.setDelay(modulatedDelay);

            float drySample = buffer.getSample(ch, s);
            float delayedSample = delay.popSample(0);

            delay.pushSample(0, drySample + delayedSample * feedbackAmount);
            buffer.setSample(ch, s, drySample * (1.0f - mixAmount) + delayedSample * mixAmount);
        }

        // Advance LFO phase once per sample
        lfoPhase += phaseInc;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    }
}

void Chorus::reset()
{
    lfoPhase = 0.0f;
    for (auto& dl : delayLines)
        dl.reset();
}

void Chorus::setRate(float rateHz)
{
    rate = juce::jlimit(0.1f, 10.0f, rateHz);
}

void Chorus::setDepth(float depthMs)
{
    depth = juce::jlimit(0.0f, 20.0f, depthMs);
}

void Chorus::setCentreDelay(float delayMs)
{
    centreDelay = juce::jlimit(1.0f, 30.0f, delayMs);
}

void Chorus::setFeedback(float feedback)
{
    feedbackAmount = juce::jlimit(0.0f, 0.9f, feedback);
}

void Chorus::setMix(float mix)
{
    mixAmount = juce::jlimit(0.0f, 1.0f, mix);
}

float Chorus::getNextLFOValue(float phaseOffset)
{
    float p = lfoPhase + phaseOffset;
    if (p >= 1.0f) p -= 1.0f;
    return std::sin(p * juce::MathConstants<float>::twoPi);
}

} // namespace LiveSystems::AudioEffects
