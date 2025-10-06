#include "DualWaveshapeProcessor.h"
#include "DualWaveshapeEditor.h"

//==============================================================================
DualWaveshapeProcessor::DualWaveshapeProcessor()
{
    initializeParameters();
}

DualWaveshapeProcessor::~DualWaveshapeProcessor()
{
}

//==============================================================================
void DualWaveshapeProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    
    // Prepare distortion channels
    distortionA.prepare(sampleRate, samplesPerBlock);
    distortionB.prepare(sampleRate, samplesPerBlock);
    
    // Prepare LFO
    lfo.prepare(sampleRate);
    
    // Allocate buffers
    channelABuffer.setSize(2, samplesPerBlock);
    channelBBuffer.setSize(2, samplesPerBlock);
    dryBuffer.setSize(2, samplesPerBlock);
}

void DualWaveshapeProcessor::releaseResources()
{
    // Release resources when playback stops
}

//==============================================================================
void DualWaveshapeProcessor::initializeParameters()
{
    using ParamInfo = ParameterManager::ParameterInfo;
    
    // ========== CHANNEL A ==========
    ParamInfo driveAInfo;
    driveAInfo.id = DRIVE_A_PARAM_ID;
    driveAInfo.name = "Drive A";
    driveAInfo.defaultValue = 0.3f;
    driveAInfo.minValue = 0.0f;
    driveAInfo.maxValue = 1.0f;
    driveAInfo.units = "";
    driveAInfo.valueToText = [](float value) { return juce::String(value * 100.0f, 1) + "%"; };
    driveAInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    parameterManager->addFloatParameter(driveAInfo);
    
    ParamInfo shapeAInfo;
    shapeAInfo.id = SHAPE_A_PARAM_ID;
    shapeAInfo.name = "Shape A";
    shapeAInfo.defaultValue = 0.0f;
    shapeAInfo.minValue = 0.0f;
    shapeAInfo.maxValue = 6.0f;
    shapeAInfo.units = "";
    shapeAInfo.valueToText = waveshapeTypeToString;
    shapeAInfo.textToValue = stringToWaveshapeType;
    parameterManager->addFloatParameter(shapeAInfo);
    
    ParamInfo mixAInfo;
    mixAInfo.id = MIX_A_PARAM_ID;
    mixAInfo.name = "Mix A";
    mixAInfo.defaultValue = 1.0f;
    mixAInfo.minValue = 0.0f;
    mixAInfo.maxValue = 1.0f;
    mixAInfo.units = "";
    mixAInfo.valueToText = [](float value) { return juce::String(value * 100.0f, 1) + "%"; };
    mixAInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    parameterManager->addFloatParameter(mixAInfo);
    
    // ========== CHANNEL B ==========
    ParamInfo driveBInfo;
    driveBInfo.id = DRIVE_B_PARAM_ID;
    driveBInfo.name = "Drive B";
    driveBInfo.defaultValue = 0.3f;
    driveBInfo.minValue = 0.0f;
    driveBInfo.maxValue = 1.0f;
    driveBInfo.units = "";
    driveBInfo.valueToText = [](float value) { return juce::String(value * 100.0f, 1) + "%"; };
    driveBInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    parameterManager->addFloatParameter(driveBInfo);
    
    ParamInfo shapeBInfo;
    shapeBInfo.id = SHAPE_B_PARAM_ID;
    shapeBInfo.name = "Shape B";
    shapeBInfo.defaultValue = 0.0f;
    shapeBInfo.minValue = 0.0f;
    shapeBInfo.maxValue = 6.0f;
    shapeBInfo.units = "";
    shapeBInfo.valueToText = waveshapeTypeToString;
    shapeBInfo.textToValue = stringToWaveshapeType;
    parameterManager->addFloatParameter(shapeBInfo);
    
    ParamInfo mixBInfo;
    mixBInfo.id = MIX_B_PARAM_ID;
    mixBInfo.name = "Mix B";
    mixBInfo.defaultValue = 1.0f;
    mixBInfo.minValue = 0.0f;
    mixBInfo.maxValue = 1.0f;
    mixBInfo.units = "";
    mixBInfo.valueToText = [](float value) { return juce::String(value * 100.0f, 1) + "%"; };
    mixBInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    parameterManager->addFloatParameter(mixBInfo);
    
    // ========== LFO ==========
    ParamInfo lfoRateInfo;
    lfoRateInfo.id = LFO_RATE_PARAM_ID;
    lfoRateInfo.name = "LFO Rate";
    lfoRateInfo.defaultValue = 1.0f;
    lfoRateInfo.minValue = 0.01f;
    lfoRateInfo.maxValue = 20.0f;
    lfoRateInfo.units = "Hz";
    lfoRateInfo.valueToText = [](float value) { return juce::String(value, 2) + " Hz"; };
    lfoRateInfo.textToValue = [](const juce::String& text) { return text.getFloatValue(); };
    parameterManager->addFloatParameter(lfoRateInfo);
    
    ParamInfo lfoDepthInfo;
    lfoDepthInfo.id = LFO_DEPTH_PARAM_ID;
    lfoDepthInfo.name = "LFO Depth";
    lfoDepthInfo.defaultValue = 0.0f;
    lfoDepthInfo.minValue = 0.0f;
    lfoDepthInfo.maxValue = 1.0f;
    lfoDepthInfo.units = "";
    lfoDepthInfo.valueToText = [](float value) { return juce::String(value * 100.0f, 1) + "%"; };
    lfoDepthInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    parameterManager->addFloatParameter(lfoDepthInfo);
    
    ParamInfo lfoWaveformInfo;
    lfoWaveformInfo.id = LFO_WAVEFORM_PARAM_ID;
    lfoWaveformInfo.name = "LFO Waveform";
    lfoWaveformInfo.defaultValue = 0.0f;
    lfoWaveformInfo.minValue = 0.0f;
    lfoWaveformInfo.maxValue = 5.0f;
    lfoWaveformInfo.units = "";
    lfoWaveformInfo.valueToText = lfoWaveformToString;
    lfoWaveformInfo.textToValue = stringToLFOWaveform;
    parameterManager->addFloatParameter(lfoWaveformInfo);
    
    ParamInfo lfoTargetInfo;
    lfoTargetInfo.id = LFO_TARGET_PARAM_ID;
    lfoTargetInfo.name = "LFO Target";
    lfoTargetInfo.defaultValue = 0.0f;
    lfoTargetInfo.minValue = 0.0f;
    lfoTargetInfo.maxValue = 5.0f;
    lfoTargetInfo.units = "";
    lfoTargetInfo.valueToText = lfoTargetToString;
    lfoTargetInfo.textToValue = stringToLFOTarget;
    parameterManager->addFloatParameter(lfoTargetInfo);
    
    // ========== MASTER ==========
    ParamInfo outputInfo;
    outputInfo.id = OUTPUT_PARAM_ID;
    outputInfo.name = "Output";
    outputInfo.defaultValue = 0.0f;
    outputInfo.minValue = -20.0f;
    outputInfo.maxValue = 20.0f;
    outputInfo.units = "dB";
    outputInfo.valueToText = [](float value) { return juce::String(value, 1) + " dB"; };
    outputInfo.textToValue = [](const juce::String& text) { return text.getFloatValue(); };
    parameterManager->addFloatParameter(outputInfo);
    
    ParamInfo balanceInfo;
    balanceInfo.id = BALANCE_PARAM_ID;
    balanceInfo.name = "A/B Balance";
    balanceInfo.defaultValue = 0.5f;
    balanceInfo.minValue = 0.0f;
    balanceInfo.maxValue = 1.0f;
    balanceInfo.units = "";
    balanceInfo.valueToText = [](float value) { 
        if (value < 0.5f) return "A " + juce::String((0.5f - value) * 200.0f, 0) + "%";
        if (value > 0.5f) return "B " + juce::String((value - 0.5f) * 200.0f, 0) + "%";
        return juce::String("Center");
    };
    balanceInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    parameterManager->addFloatParameter(balanceInfo);
    
    ParamInfo masterMixInfo;
    masterMixInfo.id = MASTER_MIX_PARAM_ID;
    masterMixInfo.name = "Master Mix";
    masterMixInfo.defaultValue = 1.0f;
    masterMixInfo.minValue = 0.0f;
    masterMixInfo.maxValue = 1.0f;
    masterMixInfo.units = "";
    masterMixInfo.valueToText = [](float value) { return juce::String(value * 100.0f, 1) + "%"; };
    masterMixInfo.textToValue = [](const juce::String& text) { return text.getFloatValue() / 100.0f; };
    parameterManager->addFloatParameter(masterMixInfo);
    
    // Create parameter groups
    parameterManager->createParameterGroup("Channel A", { DRIVE_A_PARAM_ID, SHAPE_A_PARAM_ID, MIX_A_PARAM_ID });
    parameterManager->createParameterGroup("Channel B", { DRIVE_B_PARAM_ID, SHAPE_B_PARAM_ID, MIX_B_PARAM_ID });
    parameterManager->createParameterGroup("LFO", { LFO_RATE_PARAM_ID, LFO_DEPTH_PARAM_ID, LFO_WAVEFORM_PARAM_ID, LFO_TARGET_PARAM_ID });
    parameterManager->createParameterGroup("Master", { OUTPUT_PARAM_ID, BALANCE_PARAM_ID, MASTER_MIX_PARAM_ID });
}

//==============================================================================
void DualWaveshapeProcessor::processAudio(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    if (numSamples == 0 || numChannels == 0)
        return;
    
    // Get parameters
    float driveA = parameterManager->getParameterValue(DRIVE_A_PARAM_ID);
    float driveB = parameterManager->getParameterValue(DRIVE_B_PARAM_ID);
    float mixA = parameterManager->getParameterValue(MIX_A_PARAM_ID);
    float mixB = parameterManager->getParameterValue(MIX_B_PARAM_ID);
    
    const int shapeA = static_cast<int>(parameterManager->getParameterValue(SHAPE_A_PARAM_ID));
    const int shapeB = static_cast<int>(parameterManager->getParameterValue(SHAPE_B_PARAM_ID));
    
    const float lfoRate = parameterManager->getParameterValue(LFO_RATE_PARAM_ID);
    const float lfoDepth = parameterManager->getParameterValue(LFO_DEPTH_PARAM_ID);
    const int lfoWaveform = static_cast<int>(parameterManager->getParameterValue(LFO_WAVEFORM_PARAM_ID));
    
    const float outputGain = juce::Decibels::decibelsToGain(parameterManager->getParameterValue(OUTPUT_PARAM_ID));
    const float balance = parameterManager->getParameterValue(BALANCE_PARAM_ID);
    const float masterMix = parameterManager->getParameterValue(MASTER_MIX_PARAM_ID);
    
    // Update LFO settings
    lfo.setFrequency(lfoRate);
    lfo.setWaveform(static_cast<LiveSystems::Components::Oscillators::LFO::Waveform>(lfoWaveform));
    
    // Store dry signal
    dryBuffer.makeCopyOf(buffer);
    
    // Prepare channel buffers
    channelABuffer.setSize(numChannels, numSamples, false, false, true);
    channelBBuffer.setSize(numChannels, numSamples, false, false, true);
    
    channelABuffer.makeCopyOf(buffer);
    channelBBuffer.makeCopyOf(buffer);
    
    // Process each sample with LFO modulation
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Get modulation values
        float modDriveA = driveA;
        float modDriveB = driveB;
        float modMixA = mixA;
        float modMixB = mixB;
        
        if (lfoDepth > 0.0f)
        {
            float lfoValue = lfo.getNextSample();
            float modAmount = (lfoValue - 0.5f) * 2.0f * lfoDepth;  // -depth to +depth
            
            int target = static_cast<int>(parameterManager->getParameterValue(LFO_TARGET_PARAM_ID));
            switch (static_cast<LFOTarget>(target))
            {
                case LFOTarget::DriveA:
                    modDriveA = juce::jlimit(0.0f, 1.0f, driveA + modAmount);
                    break;
                case LFOTarget::DriveB:
                    modDriveB = juce::jlimit(0.0f, 1.0f, driveB + modAmount);
                    break;
                case LFOTarget::MixA:
                    modMixA = juce::jlimit(0.0f, 1.0f, mixA + modAmount);
                    break;
                case LFOTarget::MixB:
                    modMixB = juce::jlimit(0.0f, 1.0f, mixB + modAmount);
                    break;
                case LFOTarget::BothDrives:
                    modDriveA = juce::jlimit(0.0f, 1.0f, driveA + modAmount);
                    modDriveB = juce::jlimit(0.0f, 1.0f, driveB + modAmount);
                    break;
                case LFOTarget::BothMixes:
                    modMixA = juce::jlimit(0.0f, 1.0f, mixA + modAmount);
                    modMixB = juce::jlimit(0.0f, 1.0f, mixB + modAmount);
                    break;
            }
        }
        
        // Apply modulated parameters to each channel
        for (int channel = 0; channel < numChannels; ++channel)
        {
            // Channel A
            auto* dataA = channelABuffer.getWritePointer(channel);
            distortionA.setDrive(modDriveA);
            distortionA.setMix(modMixA);
            distortionA.setWaveshapeType(static_cast<LiveSystems::Components::Distortions::WaveshapeDistortion::WaveshapeType>(shapeA));
            
            // Channel B
            auto* dataB = channelBBuffer.getWritePointer(channel);
            distortionB.setDrive(modDriveB);
            distortionB.setMix(modMixB);
            distortionB.setWaveshapeType(static_cast<LiveSystems::Components::Distortions::WaveshapeDistortion::WaveshapeType>(shapeB));
        }
    }
    
    // Process distortion channels
    distortionA.process(channelABuffer);
    distortionB.process(channelBBuffer);
    
    // Mix channels A and B based on balance
    float gainA = 1.0f - balance;
    float gainB = balance;
    
    // Apply balance and sum to output
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* outputData = buffer.getWritePointer(channel);
        const auto* dataA = channelABuffer.getReadPointer(channel);
        const auto* dataB = channelBBuffer.getReadPointer(channel);
        const auto* dryData = dryBuffer.getReadPointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float wetSample = (dataA[sample] * gainA) + (dataB[sample] * gainB);
            outputData[sample] = (dryData[sample] * (1.0f - masterMix) + wetSample * masterMix) * outputGain;
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* DualWaveshapeProcessor::createEditor()
{
    return new DualWaveshapeEditor(*this);
}

//==============================================================================
// Helper Methods

juce::String DualWaveshapeProcessor::waveshapeTypeToString(int type)
{
    switch (type)
    {
        case 0: return "Soft Clip";
        case 1: return "Hard Clip";
        case 2: return "Asymmetric";
        case 3: return "Fold";
        case 4: return "Sine";
        case 5: return "Tube";
        case 6: return "Fuzz";
        default: return "Soft Clip";
    }
}

int DualWaveshapeProcessor::stringToWaveshapeType(const juce::String& text)
{
    if (text == "Soft Clip") return 0;
    if (text == "Hard Clip") return 1;
    if (text == "Asymmetric") return 2;
    if (text == "Fold") return 3;
    if (text == "Sine") return 4;
    if (text == "Tube") return 5;
    if (text == "Fuzz") return 6;
    return 0;
}

juce::String DualWaveshapeProcessor::lfoWaveformToString(int type)
{
    switch (type)
    {
        case 0: return "Sine";
        case 1: return "Triangle";
        case 2: return "Square";
        case 3: return "Saw";
        case 4: return "Reverse Saw";
        case 5: return "Random";
        default: return "Sine";
    }
}

int DualWaveshapeProcessor::stringToLFOWaveform(const juce::String& text)
{
    if (text == "Sine") return 0;
    if (text == "Triangle") return 1;
    if (text == "Square") return 2;
    if (text == "Saw") return 3;
    if (text == "Reverse Saw") return 4;
    if (text == "Random") return 5;
    return 0;
}

juce::String DualWaveshapeProcessor::lfoTargetToString(int target)
{
    switch (target)
    {
        case 0: return "Drive A";
        case 1: return "Drive B";
        case 2: return "Mix A";
        case 3: return "Mix B";
        case 4: return "Both Drives";
        case 5: return "Both Mixes";
        default: return "Drive A";
    }
}

int DualWaveshapeProcessor::stringToLFOTarget(const juce::String& text)
{
    if (text == "Drive A") return 0;
    if (text == "Drive B") return 1;
    if (text == "Mix A") return 2;
    if (text == "Mix B") return 3;
    if (text == "Both Drives") return 4;
    if (text == "Both Mixes") return 5;
    return 0;
}

//==============================================================================
// Plugin instantiation
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DualWaveshapeProcessor();
}
