#include "DistortionBase.h"

namespace LiveSystems::Components::Distortions
{
    void DistortionBase::prepare(double sampleRate, int blockSize)
    {
        currentSampleRate = sampleRate;
        currentBlockSize = blockSize;

        // Initialize smoothed parameters
        driveSmoothed.reset(sampleRate, 0.05); // 50ms ramp time
        outputSmoothed.reset(sampleRate, 0.05);
        mixSmoothed.reset(sampleRate, 0.05);

        driveSmoothed.setCurrentAndTargetValue(driveAmount);
        outputSmoothed.setCurrentAndTargetValue(outputLevel);
        mixSmoothed.setCurrentAndTargetValue(mixAmount);
    }

    void DistortionBase::reset()
    {
        driveSmoothed.reset(currentSampleRate, 0.05);
        outputSmoothed.reset(currentSampleRate, 0.05);
        mixSmoothed.reset(currentSampleRate, 0.05);
    }

    void DistortionBase::setDrive(float drive)
    {
        driveAmount = juce::jlimit(0.0f, 1.0f, drive);
        driveSmoothed.setTargetValue(driveAmount);
    }

    void DistortionBase::setOutputLevel(float level)
    {
        outputLevel = juce::jlimit(0.0f, 1.0f, level);
        outputSmoothed.setTargetValue(outputLevel);
    }

    void DistortionBase::setMix(float mix)
    {
        mixAmount = juce::jlimit(0.0f, 1.0f, mix);
        mixSmoothed.setTargetValue(mixAmount);
    }

} // namespace LiveSystems::Components::Distortions