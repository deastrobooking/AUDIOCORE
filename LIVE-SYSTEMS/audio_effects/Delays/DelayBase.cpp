#include "DelayBase.h"

namespace LiveSystems::AudioEffects::Delays
{

void DelayBase::prepare(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;

    delayTimeSmoothed.reset(sampleRate, 0.05);
    delayTimeSmoothed.setCurrentAndTargetValue(delayTimeMs);

    feedbackSmoothed.reset(sampleRate, 0.05);
    feedbackSmoothed.setCurrentAndTargetValue(feedbackAmount);

    mixSmoothed.reset(sampleRate, 0.05);
    mixSmoothed.setCurrentAndTargetValue(mixAmount);
}

void DelayBase::reset()
{
    delayTimeSmoothed.setCurrentAndTargetValue(delayTimeMs);
    feedbackSmoothed.setCurrentAndTargetValue(feedbackAmount);
    mixSmoothed.setCurrentAndTargetValue(mixAmount);
}

void DelayBase::setDelayTime(float timeMs)
{
    delayTimeMs = juce::jlimit(0.0f, 2000.0f, timeMs);
    delayTimeSmoothed.setTargetValue(delayTimeMs);
}

void DelayBase::setFeedback(float feedback)
{
    feedbackAmount = juce::jlimit(0.0f, 0.95f, feedback);
    feedbackSmoothed.setTargetValue(feedbackAmount);
}

void DelayBase::setMix(float mix)
{
    mixAmount = juce::jlimit(0.0f, 1.0f, mix);
    mixSmoothed.setTargetValue(mixAmount);
}

} // namespace LiveSystems::AudioEffects::Delays
