#include "PingPongDelay.h"

namespace LiveSystems::AudioEffects::Delays
{

void PingPongDelay::prepare(double sampleRate, int blockSize)
{
    DelayBase::prepare(sampleRate, blockSize);

    int maxDelaySamples = static_cast<int>(sampleRate * maxDelaySeconds);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(blockSize);
    spec.numChannels = 1;

    delayLeft = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>(maxDelaySamples);
    delayRight = juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>(maxDelaySamples);

    delayLeft.prepare(spec);
    delayRight.prepare(spec);
    delayLeft.reset();
    delayRight.reset();
}

void PingPongDelay::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    if (numChannels < 1 || numSamples == 0) return;

    float delaySamples = msToSamples(delayTimeMs);
    delayLeft.setDelay(delaySamples);
    delayRight.setDelay(delaySamples);

    auto* left  = buffer.getWritePointer(0);
    auto* right = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int s = 0; s < numSamples; ++s)
    {
        float dryL = left[s];
        float dryR = right ? right[s] : dryL;

        // Read from delay lines
        float delL = delayLeft.popSample(0);
        float delR = delayRight.popSample(0);

        // Cross-feed: left delay feeds into right and vice-versa (ping-pong)
        delayLeft.pushSample(0, dryL + delR * feedbackAmount);
        delayRight.pushSample(0, dryR + delL * feedbackAmount);

        // Wet/dry mix
        left[s] = dryL * (1.0f - mixAmount) + delL * mixAmount;
        if (right)
            right[s] = dryR * (1.0f - mixAmount) + delR * mixAmount;
    }
}

void PingPongDelay::reset()
{
    DelayBase::reset();
    delayLeft.reset();
    delayRight.reset();
}

} // namespace LiveSystems::AudioEffects::Delays
