#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace LiveSystems::Components::Distortions
{
    /**
     * @brief Base class for all distortion components
     * 
     * Provides common interface and functionality for distortion effects.
     * All distortion components should inherit from this class.
     */
    class DistortionBase
    {
    public:
        DistortionBase() = default;
        virtual ~DistortionBase() = default;

        /**
         * @brief Initialize the distortion component
         * @param sampleRate The sample rate to use
         * @param blockSize The maximum expected block size
         */
        virtual void prepare(double sampleRate, int blockSize);

        /**
         * @brief Process an audio buffer
         * @param buffer The audio buffer to process
         */
        virtual void process(juce::AudioBuffer<float>& buffer) = 0;

        /**
         * @brief Reset the internal state
         */
        virtual void reset();

        /**
         * @brief Set the drive/intensity parameter
         * @param drive Drive amount (0.0 to 1.0)
         */
        virtual void setDrive(float drive);

        /**
         * @brief Set the output level
         * @param level Output level (0.0 to 1.0)
         */
        virtual void setOutputLevel(float level);

        /**
         * @brief Set the dry/wet mix
         * @param mix Mix amount (0.0 = dry, 1.0 = wet)
         */
        virtual void setMix(float mix);

    protected:
        double currentSampleRate = 44100.0;
        int currentBlockSize = 512;
        
        float driveAmount = 0.5f;
        float outputLevel = 0.7f;
        float mixAmount = 1.0f;

        // Common smoothed parameters
        juce::SmoothedValue<float> driveSmoothed;
        juce::SmoothedValue<float> outputSmoothed;
        juce::SmoothedValue<float> mixSmoothed;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionBase)
    };

} // namespace LiveSystems::Components::Distortions