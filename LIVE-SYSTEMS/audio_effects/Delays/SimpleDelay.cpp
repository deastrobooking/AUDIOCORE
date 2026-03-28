#include "SimpleDelay.h"

namespace LiveSystems::AudioEffects::Delays
{

void SimpleDelay::prepare(double sampleRate, int blockSize)
{
    DelayBase::prepare(sampleRate, blockSize);

    int maxDelaySamples = static_cast<int>(sampleRate * maxDelaySeconds);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = 1; // each delay line is mono

    delayLines.clear();
    // Pre-allocate for stereo; extra channels handled gracefully
    for (int ch = 0; ch < 2; ++ch)
    {
        delayLines.emplace_back(maxDelaySamples);
        delayLines.back().prepare(spec);
        delayLines.back().reset();
    }
}

void SimpleDelay::process(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Ensure we have enough delay lines
    while (static_cast<int>(delayLines.size()) < numChannels)
    {
        int maxDelaySamples = static_cast<int>(currentSampleRate * maxDelaySeconds);
        delayLines.emplace_back(maxDelaySamples);
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = currentSampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(currentBlockSize);
        spec.numChannels = 1;
        delayLines.back().prepare(spec);
    }

    float delaySamples = msToSamples(delayTimeMs);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& delay = delayLines[static_cast<size_t>(ch)];

        delay.setDelay(delaySamples);

        for (int s = 0; s < numSamples; ++s)
        {
            float drySample = data[s];
            float delayedSample = delay.popSample(0);
            delay.pushSample(0, drySample + delayedSample * feedbackAmount);
            data[s] = drySample * (1.0f - mixAmount) + delayedSample * mixAmount;
        }
    }
}

void SimpleDelay::reset()
{
    DelayBase::reset();
    for (auto& dl : delayLines)
        dl.reset();
}

} // namespace LiveSystems::AudioEffects::Delays
