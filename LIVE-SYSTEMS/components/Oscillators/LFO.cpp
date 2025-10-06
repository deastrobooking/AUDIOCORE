#include "LFO.h"
#include <cmath>

namespace LiveSystems::Components::Oscillators
{
    void LFO::prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        phaseIncrement = frequency * juce::MathConstants<double>::twoPi / sampleRate;
        reset();
    }

    void LFO::reset()
    {
        phase = phaseOffset;
    }

    float LFO::getNextSample()
    {
        float output = generateWaveform();
        
        // Advance phase
        phase += phaseIncrement;
        if (phase >= juce::MathConstants<double>::twoPi)
            phase -= juce::MathConstants<double>::twoPi;
        
        return output;
    }

    void LFO::setFrequency(float frequencyHz)
    {
        frequency = juce::jlimit(0.01f, 20.0f, frequencyHz);
        phaseIncrement = frequency * juce::MathConstants<double>::twoPi / sampleRate;
    }

    void LFO::setWaveform(Waveform waveform)
    {
        currentWaveform = waveform;
    }

    void LFO::setPhaseOffset(float offset)
    {
        phaseOffset = offset;
    }

    float LFO::generateWaveform()
    {
        switch (currentWaveform)
        {
            case Waveform::Sine:
                return generateSine();
            case Waveform::Triangle:
                return generateTriangle();
            case Waveform::Square:
                return generateSquare();
            case Waveform::Saw:
                return generateSaw();
            case Waveform::ReverseSaw:
                return generateReverseSaw();
            case Waveform::Random:
                return generateRandom();
            default:
                return generateSine();
        }
    }

    float LFO::generateSine()
    {
        return (std::sin(static_cast<float>(phase)) + 1.0f) * 0.5f;
    }

    float LFO::generateTriangle()
    {
        float value = static_cast<float>(phase) / juce::MathConstants<float>::twoPi;
        return 1.0f - std::abs(value * 4.0f - 2.0f);
    }

    float LFO::generateSquare()
    {
        return phase < juce::MathConstants<double>::pi ? 1.0f : 0.0f;
    }

    float LFO::generateSaw()
    {
        return static_cast<float>(phase) / juce::MathConstants<float>::twoPi;
    }

    float LFO::generateReverseSaw()
    {
        return 1.0f - (static_cast<float>(phase) / juce::MathConstants<float>::twoPi);
    }

    float LFO::generateRandom()
    {
        // Sample & hold random
        static float lastRandom = 0.5f;
        static double lastPhase = 0.0;
        
        if (phase < lastPhase)  // Phase wrapped
            lastRandom = random.nextFloat();
        
        lastPhase = phase;
        return lastRandom;
    }
}
