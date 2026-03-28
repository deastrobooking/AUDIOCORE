#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace LiveSystems::Components::Filters
{
    /**
     * @brief Base class for all filter components.
     *
     * Provides common interface and smoothed parameters for filter effects.
     * All filter components should inherit from this class.
     */
    class FilterBase
    {
    public:
        FilterBase() = default;
        virtual ~FilterBase() = default;

        /**
         * @brief Initialize the filter component.
         * @param sampleRate The sample rate to use
         * @param blockSize The maximum expected block size
         */
        virtual void prepare(double sampleRate, int blockSize);

        /**
         * @brief Process an audio buffer through the filter.
         * @param buffer The audio buffer to process
         */
        virtual void process(juce::AudioBuffer<float>& buffer) = 0;

        /**
         * @brief Reset the internal state of the filter.
         */
        virtual void reset();

        /**
         * @brief Set the cutoff frequency in Hz.
         * @param frequency Cutoff frequency (20–20000 Hz)
         */
        virtual void setFrequency(float frequency);

        /**
         * @brief Set the resonance / Q factor.
         * @param resonance Resonance amount (0.1–10.0)
         */
        virtual void setResonance(float resonance);

        /**
         * @brief Set the dry/wet mix.
         * @param mix Mix amount (0.0 = dry, 1.0 = wet)
         */
        virtual void setMix(float mix);

    protected:
        double currentSampleRate = 44100.0;
        int currentBlockSize = 512;

        float frequencyValue = 1000.0f;
        float resonanceValue = 0.707f;
        float mixAmount = 1.0f;

        juce::SmoothedValue<float> frequencySmoothed;
        juce::SmoothedValue<float> resonanceSmoothed;
        juce::SmoothedValue<float> mixSmoothed;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterBase)
    };

} // namespace LiveSystems::Components::Filters
