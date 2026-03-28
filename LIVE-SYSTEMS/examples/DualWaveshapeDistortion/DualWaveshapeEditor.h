#pragma once

#include "DualWaveshapeProcessor.h"
#include "../../include/PluginEditor.h"

//==============================================================================
/**
    GUI Editor for Dual Waveshape Distortion
    
    Features modern, intuitive interface with:
    - Two distortion channel controls
    - LFO visualization and controls
    - Real-time parameter displays
    - Master output section
*/
class DualWaveshapeEditor : public LiveSystemsEditor
{
public:
    explicit DualWaveshapeEditor(DualWaveshapeProcessor& p);
    ~DualWaveshapeEditor() override;

    //==============================================================================
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // LiveSystemsEditor pure virtual functions
    void setupComponents() override;
    void paintBackground(juce::Graphics& g) override;
    void layoutComponents() override;

private:
    DualWaveshapeProcessor& processor;
    
    //==============================================================================
    // Channel A Controls
    juce::Slider driveASlider;
    juce::Label driveALabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAAttachment;
    
    juce::ComboBox shapeACombo;
    juce::Label shapeALabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> shapeAAttachment;
    
    juce::Slider mixASlider;
    juce::Label mixALabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAAttachment;
    
    //==============================================================================
    // Channel B Controls
    juce::Slider driveBSlider;
    juce::Label driveBLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveBAttachment;
    
    juce::ComboBox shapeBCombo;
    juce::Label shapeBLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> shapeBAttachment;
    
    juce::Slider mixBSlider;
    juce::Label mixBLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixBAttachment;
    
    //==============================================================================
    // LFO Controls
    juce::Slider lfoRateSlider;
    juce::Label lfoRateLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoRateAttachment;
    
    juce::Slider lfoDepthSlider;
    juce::Label lfoDepthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfoDepthAttachment;
    
    juce::ComboBox lfoWaveformCombo;
    juce::Label lfoWaveformLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoWaveformAttachment;
    
    juce::ComboBox lfoTargetCombo;
    juce::Label lfoTargetLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfoTargetAttachment;
    
    //==============================================================================
    // Master Controls
    juce::Slider outputSlider;
    juce::Label outputLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    
    juce::Slider balanceSlider;
    juce::Label balanceLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> balanceAttachment;
    
    juce::Slider masterMixSlider;
    juce::Label masterMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterMixAttachment;
    
    //==============================================================================
    // Preset Controls
    juce::ComboBox presetCombo;
    juce::Label presetLabel;
    juce::TextButton savePresetButton { "Save" };
    juce::TextButton deletePresetButton { "Delete" };
    
    void refreshPresetList();
    void saveCurrentPreset();
    void deleteCurrentPreset();
    
    //==============================================================================
    // Helper Methods
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    void setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualWaveshapeEditor)
};
