#pragma once

#include "FilterBase.h"

namespace LiveSystems::Components::Filters
{
    /**
     * @brief Band-pass filter using JUCE StateVariableTPT topology.
     *
     * Passes frequencies around the cutoff and attenuates both lower and higher frequencies.
     */
    class BandPassFilter : public FilterBase
    {
    public:
        BandPassFilter() = default;
        ~BandPassFilter() override = default;

        void prepare(double sampleRate, int blockSize) override;
        void process(juce::AudioBuffer<float>& buffer) override;
        void reset() override;

    private:
        juce::dsp::StateVariableTPTFilter<float> filter;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandPassFilter)
    };

} // namespace LiveSystems::Components::Filters
