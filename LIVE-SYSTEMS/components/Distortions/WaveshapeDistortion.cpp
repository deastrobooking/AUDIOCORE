#include "WaveshapeDistortion.h"
#include <cmath>

namespace LiveSystems::Components::Distortions
{
    void WaveshapeDistortion::process(juce::AudioBuffer<float>& buffer)
    {
        auto numChannels = buffer.getNumChannels();
        auto numSamples = buffer.getNumSamples();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);

            for (int sample = 0; sample < numSamples; ++sample)
            {
                auto drive = driveSmoothed.getNextValue();
                auto mix = mixSmoothed.getNextValue();
                auto output = outputSmoothed.getNextValue();

                auto inputSample = channelData[sample];
                
                // Apply selected waveshaping algorithm
                float wetSample = 0.0f;
                switch (waveshapeType)
                {
                    case WaveshapeType::SoftClip:
                        wetSample = applySoftClip(inputSample, drive);
                        break;
                    case WaveshapeType::HardClip:
                        wetSample = applyHardClip(inputSample, drive);
                        break;
                    case WaveshapeType::Asymmetric:
                        wetSample = applyAsymmetric(inputSample, drive);
                        break;
                    case WaveshapeType::Fold:
                        wetSample = applyFold(inputSample, drive);
                        break;
                    case WaveshapeType::Sine:
                        wetSample = applySine(inputSample, drive);
                        break;
                    case WaveshapeType::Tube:
                        wetSample = applyTube(inputSample, drive);
                        break;
                    case WaveshapeType::Fuzz:
                        wetSample = applyFuzz(inputSample, drive);
                        break;
                }
                
                auto mixedSample = (inputSample * (1.0f - mix)) + (wetSample * mix);
                channelData[sample] = mixedSample * output;
            }
        }
    }
    
    void WaveshapeDistortion::setWaveshapeType(WaveshapeType type)
    {
        waveshapeType = type;
    }
    
    float WaveshapeDistortion::applySoftClip(float input, float drive)
    {
        return std::tanh(input * (1.0f + drive * 9.0f));
    }
    
    float WaveshapeDistortion::applyHardClip(float input, float drive)
    {
        float driven = input * (1.0f + drive * 9.0f);
        return juce::jlimit(-1.0f, 1.0f, driven);
    }
    
    float WaveshapeDistortion::applyAsymmetric(float input, float drive)
    {
        float driven = input * (1.0f + drive * 9.0f);
        if (driven > 0.0f)
            return std::tanh(driven * 1.5f);
        else
            return std::tanh(driven * 0.75f);
    }
    
    float WaveshapeDistortion::applyFold(float input, float drive)
    {
        float driven = input * (1.0f + drive * 9.0f);
        while (driven > 1.0f)
            driven = 2.0f - driven;
        while (driven < -1.0f)
            driven = -2.0f - driven;
        return driven;
    }
    
    float WaveshapeDistortion::applySine(float input, float drive)
    {
        float driven = input * (1.0f + drive * 9.0f);
        return std::sin(driven * juce::MathConstants<float>::halfPi);
    }
    
    float WaveshapeDistortion::applyTube(float input, float drive)
    {
        float driven = input * (1.0f + drive * 9.0f);
        float q = 1.0f / std::atan(10.0f);
        return std::atan(driven * 10.0f) * q;
    }
    
    float WaveshapeDistortion::applyFuzz(float input, float drive)
    {
        float driven = input * (1.0f + drive * 19.0f);
        return driven / (1.0f + std::abs(driven));
    }
}

