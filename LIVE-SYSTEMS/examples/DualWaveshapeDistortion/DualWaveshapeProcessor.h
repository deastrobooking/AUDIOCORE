#pragma once

#include "../../include/PluginProcessor.h"
#include "../../components/Distortions/WaveshapeDistortion.h"
#include "../../components/Oscillators/LFO.h"

//==============================================================================
/**
    Dual Waveshaping Distortion with LFO Modulation
    
    Features:
    - Two parallel distortion channels (A & B)
    - 7 waveshape types per channel
    - LFO modulation with 6 waveforms
    - Flexible modulation routing
    - Stereo processing
    - Full VST3 automation support
*/
class DualWaveshapeProcessor : public LiveSystemsProcessor
{
public:
    //==============================================================================
    DualWaveshapeProcessor();
    ~DualWaveshapeProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void initializeParameters() override;
    void processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    juce::String getPluginName() const override { return "Dual Waveshape Distortion"; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

private:
    //==============================================================================
    // Parameter IDs
    
    // Channel A
    static constexpr const char* DRIVE_A_PARAM_ID = "driveA";
    static constexpr const char* SHAPE_A_PARAM_ID = "shapeA";
    static constexpr const char* MIX_A_PARAM_ID = "mixA";
    
    // Channel B
    static constexpr const char* DRIVE_B_PARAM_ID = "driveB";
    static constexpr const char* SHAPE_B_PARAM_ID = "shapeB";
    static constexpr const char* MIX_B_PARAM_ID = "mixB";
    
    // LFO
    static constexpr const char* LFO_RATE_PARAM_ID = "lfoRate";
    static constexpr const char* LFO_DEPTH_PARAM_ID = "lfoDepth";
    static constexpr const char* LFO_WAVEFORM_PARAM_ID = "lfoWaveform";
    static constexpr const char* LFO_TARGET_PARAM_ID = "lfoTarget";
    
    // Master
    static constexpr const char* OUTPUT_PARAM_ID = "output";
    static constexpr const char* BALANCE_PARAM_ID = "balance";
    static constexpr const char* MASTER_MIX_PARAM_ID = "masterMix";

    //==============================================================================
    // LFO Modulation Targets
    enum class LFOTarget
    {
        DriveA = 0,
        DriveB,
        MixA,
        MixB,
        BothDrives,
        BothMixes
    };

    //==============================================================================
    // DSP Components
    LiveSystems::Components::Distortions::WaveshapeDistortion distortionA;
    LiveSystems::Components::Distortions::WaveshapeDistortion distortionB;
    LiveSystems::Components::Oscillators::LFO lfo;
    
    // Buffers for parallel processing
    juce::AudioBuffer<float> channelABuffer;
    juce::AudioBuffer<float> channelBBuffer;
    juce::AudioBuffer<float> dryBuffer;
    
    // Current sample rate
    double currentSampleRate = 44100.0;
    
    //==============================================================================
    // Helper methods
    void processDistortionChannel(
        LiveSystems::Components::Distortions::WaveshapeDistortion& distortion,
        juce::AudioBuffer<float>& buffer,
        float drive,
        float mix,
        int waveshapeType);
    
    void applyLFOModulation(float& driveA, float& driveB, float& mixA, float& mixB);
    
    static juce::String waveshapeTypeToString(int type);
    static int stringToWaveshapeType(const juce::String& text);
    
    static juce::String lfoWaveformToString(int type);
    static int stringToLFOWaveform(const juce::String& text);
    
    static juce::String lfoTargetToString(int target);
    static int stringToLFOTarget(const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualWaveshapeProcessor)
};
