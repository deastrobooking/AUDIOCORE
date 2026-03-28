#pragma once

#include "FilterBase.h"

namespace LiveSystems::Components::Filters
{
    /**
     * @brief Low-pass filter using JUCE StateVariableTPT topology.
     *
     * Passes frequencies below the cutoff and attenuates higher frequencies.
     */
    class LowPassFilter : public FilterBase
    {
    public:
        LowPassFilter() = default;
        ~LowPassFilter() override = default;

        void prepare(double sampleRate, int blockSize) override;
        void process(juce::AudioBuffer<float>& buffer) override;
        void reset() override;

    private:
        juce::dsp::StateVariableTPTFilter<float> filter;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowPassFilter)
    };

} // namespace LiveSystems::Components::Filters
