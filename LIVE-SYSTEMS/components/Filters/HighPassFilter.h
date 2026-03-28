#pragma once

#include "FilterBase.h"

namespace LiveSystems::Components::Filters
{
    /**
     * @brief High-pass filter using JUCE StateVariableTPT topology.
     *
     * Passes frequencies above the cutoff and attenuates lower frequencies.
     */
    class HighPassFilter : public FilterBase
    {
    public:
        HighPassFilter() = default;
        ~HighPassFilter() override = default;

        void prepare(double sampleRate, int blockSize) override;
        void process(juce::AudioBuffer<float>& buffer) override;
        void reset() override;

    private:
        juce::dsp::StateVariableTPTFilter<float> filter;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighPassFilter)
    };

} // namespace LiveSystems::Components::Filters
