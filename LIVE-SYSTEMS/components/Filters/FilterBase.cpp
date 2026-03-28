#include "FilterBase.h"

namespace LiveSystems::Components::Filters
{

void FilterBase::prepare(double sampleRate, int blockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize = blockSize;

    frequencySmoothed.reset(sampleRate, 0.05);
    frequencySmoothed.setCurrentAndTargetValue(frequencyValue);

    resonanceSmoothed.reset(sampleRate, 0.05);
    resonanceSmoothed.setCurrentAndTargetValue(resonanceValue);

    mixSmoothed.reset(sampleRate, 0.05);
    mixSmoothed.setCurrentAndTargetValue(mixAmount);
}

void FilterBase::reset()
{
    frequencySmoothed.setCurrentAndTargetValue(frequencyValue);
    resonanceSmoothed.setCurrentAndTargetValue(resonanceValue);
    mixSmoothed.setCurrentAndTargetValue(mixAmount);
}

void FilterBase::setFrequency(float frequency)
{
    frequencyValue = juce::jlimit(20.0f, 20000.0f, frequency);
    frequencySmoothed.setTargetValue(frequencyValue);
}

void FilterBase::setResonance(float resonance)
{
    resonanceValue = juce::jlimit(0.1f, 10.0f, resonance);
    resonanceSmoothed.setTargetValue(resonanceValue);
}

void FilterBase::setMix(float mix)
{
    mixAmount = juce::jlimit(0.0f, 1.0f, mix);
    mixSmoothed.setTargetValue(mixAmount);
}

} // namespace LiveSystems::Components::Filters
