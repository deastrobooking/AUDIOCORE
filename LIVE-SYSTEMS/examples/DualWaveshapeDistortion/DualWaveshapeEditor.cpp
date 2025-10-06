#include "DualWaveshapeEditor.h"

//==============================================================================
DualWaveshapeEditor::DualWaveshapeEditor(DualWaveshapeProcessor& p)
    : LiveSystemsEditor(p), processor(p)
{
    // Set editor size
    setSize(800, 600);
    
    // ========== CHANNEL A ==========
    setupSlider(driveASlider, driveALabel, "Drive A");
    driveASlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveASlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    driveAAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "driveA", driveASlider));
    
    setupComboBox(shapeACombo, shapeALabel, "Shape A");
    shapeACombo.addItem("Soft Clip", 1);
    shapeACombo.addItem("Hard Clip", 2);
    shapeACombo.addItem("Asymmetric", 3);
    shapeACombo.addItem("Fold", 4);
    shapeACombo.addItem("Sine", 5);
    shapeACombo.addItem("Tube", 6);
    shapeACombo.addItem("Fuzz", 7);
    shapeAAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        processor.getParameterManager()->getValueTreeState(), "shapeA", shapeACombo));
    
    setupSlider(mixASlider, mixALabel, "Mix A");
    mixASlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixASlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixAAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "mixA", mixASlider));
    
    // ========== CHANNEL B ==========
    setupSlider(driveBSlider, driveBLabel, "Drive B");
    driveBSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    driveBSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    driveBAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "driveB", driveBSlider));
    
    setupComboBox(shapeBCombo, shapeBLabel, "Shape B");
    shapeBCombo.addItem("Soft Clip", 1);
    shapeBCombo.addItem("Hard Clip", 2);
    shapeBCombo.addItem("Asymmetric", 3);
    shapeBCombo.addItem("Fold", 4);
    shapeBCombo.addItem("Sine", 5);
    shapeBCombo.addItem("Tube", 6);
    shapeBCombo.addItem("Fuzz", 7);
    shapeBAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        processor.getParameterManager()->getValueTreeState(), "shapeB", shapeBCombo));
    
    setupSlider(mixBSlider, mixBLabel, "Mix B");
    mixBSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mixBSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixBAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "mixB", mixBSlider));
    
    // ========== LFO ==========
    setupSlider(lfoRateSlider, lfoRateLabel, "LFO Rate");
    lfoRateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lfoRateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    lfoRateAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "lfoRate", lfoRateSlider));
    
    setupSlider(lfoDepthSlider, lfoDepthLabel, "LFO Depth");
    lfoDepthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lfoDepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    lfoDepthAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "lfoDepth", lfoDepthSlider));
    
    setupComboBox(lfoWaveformCombo, lfoWaveformLabel, "LFO Waveform");
    lfoWaveformCombo.addItem("Sine", 1);
    lfoWaveformCombo.addItem("Triangle", 2);
    lfoWaveformCombo.addItem("Square", 3);
    lfoWaveformCombo.addItem("Saw", 4);
    lfoWaveformCombo.addItem("Reverse Saw", 5);
    lfoWaveformCombo.addItem("Random", 6);
    lfoWaveformAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        processor.getParameterManager()->getValueTreeState(), "lfoWaveform", lfoWaveformCombo));
    
    setupComboBox(lfoTargetCombo, lfoTargetLabel, "LFO Target");
    lfoTargetCombo.addItem("Drive A", 1);
    lfoTargetCombo.addItem("Drive B", 2);
    lfoTargetCombo.addItem("Mix A", 3);
    lfoTargetCombo.addItem("Mix B", 4);
    lfoTargetCombo.addItem("Both Drives", 5);
    lfoTargetCombo.addItem("Both Mixes", 6);
    lfoTargetAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        processor.getParameterManager()->getValueTreeState(), "lfoTarget", lfoTargetCombo));
    
    // ========== MASTER ==========
    setupSlider(outputSlider, outputLabel, "Output");
    outputSlider.setSliderStyle(juce::Slider::LinearVertical);
    outputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    outputAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "output", outputSlider));
    
    setupSlider(balanceSlider, balanceLabel, "A/B Balance");
    balanceSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    balanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    balanceAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "balance", balanceSlider));
    
    setupSlider(masterMixSlider, masterMixLabel, "Master Mix");
    masterMixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    masterMixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    masterMixAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        processor.getParameterManager()->getValueTreeState(), "masterMix", masterMixSlider));
}

DualWaveshapeEditor::~DualWaveshapeEditor()
{
}

//==============================================================================
void DualWaveshapeEditor::paint(juce::Graphics& g)
{
    // Background
    g.fillAll(juce::Colour(0xff1e1e1e));
    
    // Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawText("DUAL WAVESHAPE DISTORTION", 0, 10, getWidth(), 40, juce::Justification::centred);
    
    // Section backgrounds
    g.setColour(juce::Colour(0xff2d2d2d));
    
    // Channel A section
    g.fillRoundedRectangle(20.0f, 60.0f, 240.0f, 240.0f, 10.0f);
    g.setColour(juce::Colours::white);
    g.setFont(18.0f);
    g.drawText("CHANNEL A", 20, 60, 240, 30, juce::Justification::centred);
    
    // Channel B section
    g.setColour(juce::Colour(0xff2d2d2d));
    g.fillRoundedRectangle(280.0f, 60.0f, 240.0f, 240.0f, 10.0f);
    g.setColour(juce::Colours::white);
    g.drawText("CHANNEL B", 280, 60, 240, 30, juce::Justification::centred);
    
    // LFO section
    g.setColour(juce::Colour(0xff2d2d2d));
    g.fillRoundedRectangle(540.0f, 60.0f, 240.0f, 240.0f, 10.0f);
    g.setColour(juce::Colours::white);
    g.drawText("LFO MODULATION", 540, 60, 240, 30, juce::Justification::centred);
    
    // Master section
    g.setColour(juce::Colour(0xff2d2d2d));
    g.fillRoundedRectangle(20.0f, 320.0f, 760.0f, 260.0f, 10.0f);
    g.setColour(juce::Colours::white);
    g.drawText("MASTER OUTPUT", 20, 320, 760, 30, juce::Justification::centred);
}

void DualWaveshapeEditor::resized()
{
    // Channel A
    driveALabel.setBounds(40, 95, 80, 20);
    driveASlider.setBounds(40, 115, 80, 80);
    
    shapeALabel.setBounds(140, 95, 100, 20);
    shapeACombo.setBounds(140, 115, 100, 25);
    
    mixALabel.setBounds(40, 205, 80, 20);
    mixASlider.setBounds(40, 225, 80, 80);
    
    // Channel B
    driveBLabel.setBounds(300, 95, 80, 20);
    driveBSlider.setBounds(300, 115, 80, 80);
    
    shapeBLabel.setBounds(400, 95, 100, 20);
    shapeBCombo.setBounds(400, 115, 100, 25);
    
    mixBLabel.setBounds(300, 205, 80, 20);
    mixBSlider.setBounds(300, 225, 80, 80);
    
    // LFO
    lfoRateLabel.setBounds(560, 95, 80, 20);
    lfoRateSlider.setBounds(560, 115, 80, 80);
    
    lfoDepthLabel.setBounds(660, 95, 80, 20);
    lfoDepthSlider.setBounds(660, 115, 80, 80);
    
    lfoWaveformLabel.setBounds(560, 205, 100, 20);
    lfoWaveformCombo.setBounds(560, 225, 100, 25);
    
    lfoTargetLabel.setBounds(560, 260, 100, 20);
    lfoTargetCombo.setBounds(560, 280, 100, 25);
    
    // Master
    outputLabel.setBounds(680, 360, 60, 20);
    outputSlider.setBounds(690, 380, 40, 160);
    
    balanceLabel.setBounds(40, 360, 600, 20);
    balanceSlider.setBounds(40, 380, 600, 40);
    
    masterMixLabel.setBounds(40, 450, 600, 20);
    masterMixSlider.setBounds(40, 470, 600, 40);
}

//==============================================================================
void DualWaveshapeEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(slider);
    
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
}

void DualWaveshapeEditor::setupComboBox(juce::ComboBox& combo, juce::Label& label, const juce::String& labelText)
{
    addAndMakeVisible(combo);
    
    addAndMakeVisible(label);
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&combo, false);
}
