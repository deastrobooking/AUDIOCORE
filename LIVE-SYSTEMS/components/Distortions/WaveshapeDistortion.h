#pragma once

#include "DistortionBase.h"

namespace LiveSystems::Components::Distortions
{
    class WaveshapeDistortion : public DistortionBase
    {
    public:
        enum class WaveshapeType
        {
            SoftClip,      // Tanh soft clipping
            HardClip,      // Hard clipping
            Asymmetric,    // Asymmetric waveshaping
            Fold,          // Wavefolding
            Sine,          // Sine waveshaping
            Tube,          // Tube-like distortion
            Fuzz           // Fuzz distortion
        };

        WaveshapeDistortion() = default;
        ~WaveshapeDistortion() override = default;

        void process(juce::AudioBuffer<float>& buffer) override;
        
        /**
         * @brief Set the waveshape type
         * @param type The waveshape algorithm to use
         */
        void setWaveshapeType(WaveshapeType type);
        
        /**
         * @brief Get the current waveshape type
         */
        WaveshapeType getWaveshapeType() const { return waveshapeType; }

    private:
        WaveshapeType waveshapeType = WaveshapeType::SoftClip;
        
        float applySoftClip(float input, float drive);
        float applyHardClip(float input, float drive);
        float applyAsymmetric(float input, float drive);
        float applyFold(float input, float drive);
        float applySine(float input, float drive);
        float applyTube(float input, float drive);
        float applyFuzz(float input, float drive);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshapeDistortion)
    };
}

