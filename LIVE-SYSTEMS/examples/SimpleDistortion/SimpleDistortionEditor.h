#pragma once

#include "../../include/PluginEditor.h"
#include "SimpleDistortionProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

//==============================================================================
/**
    Simple distortion plugin editor that demonstrates modern GUI design with the LIVE-SYSTEMS framework.
*/
class SimpleDistortionEditor : public LiveSystemsEditor
{
public:
    SimpleDistortionEditor(SimpleDistortionProcessor&);
    ~SimpleDistortionEditor() override;

protected:
    //==============================================================================
    // Framework overrides
    void setupComponents() override;
    void paintBackground(juce::Graphics& g) override;
    void layoutComponents() override;

private:
    //==============================================================================
    // GUI Components
    juce::Slider driveSlider;
    juce::Slider outputSlider;
    juce::Slider mixSlider;
    
    juce::Label driveLabel;
    juce::Label outputLabel;
    juce::Label mixLabel;
    
    juce::Label titleLabel;
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    SimpleDistortionProcessor& getProcessor() { return static_cast<SimpleDistortionProcessor&>(audioProcessor); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleDistortionEditor)
};