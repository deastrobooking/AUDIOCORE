#include "SimpleDistortionEditor.h"

//==============================================================================
SimpleDistortionEditor::SimpleDistortionEditor(SimpleDistortionProcessor& p)
    : LiveSystemsEditor(p)
{
    // Set initial size
    setSize(400, 300);
    
    // Setup components after base class construction is complete
    setupComponents();
}

SimpleDistortionEditor::~SimpleDistortionEditor()
{
}

//==============================================================================
void SimpleDistortionEditor::setupComponents()
{
    // Setup title
    titleLabel.setText("Simple Distortion", juce::dontSendNotification);
    titleLabel.setFont(currentTheme.headerFont);
    titleLabel.setColour(juce::Label::textColourId, currentTheme.textColour);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Setup Drive slider
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    driveSlider.setRange(1.0, 10.0, 0.1);
    driveSlider.setValue(2.0);
    driveSlider.setColour(juce::Slider::rotarySliderFillColourId, currentTheme.primaryColour);
    driveSlider.setColour(juce::Slider::rotarySliderOutlineColourId, currentTheme.outlineColour);
    driveSlider.setColour(juce::Slider::textBoxTextColourId, currentTheme.textColour);
    driveSlider.setColour(juce::Slider::textBoxBackgroundColourId, currentTheme.secondaryColour);
    addAndMakeVisible(driveSlider);

    driveLabel.setText("Drive", juce::dontSendNotification);
    driveLabel.setFont(currentTheme.primaryFont);
    driveLabel.setColour(juce::Label::textColourId, currentTheme.textColour);
    driveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(driveLabel);

    // Setup Output slider
    outputSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    outputSlider.setRange(-20.0, 20.0, 0.1);
    outputSlider.setValue(0.0);
    outputSlider.setColour(juce::Slider::rotarySliderFillColourId, currentTheme.primaryColour);
    outputSlider.setColour(juce::Slider::rotarySliderOutlineColourId, currentTheme.outlineColour);
    outputSlider.setColour(juce::Slider::textBoxTextColourId, currentTheme.textColour);
    outputSlider.setColour(juce::Slider::textBoxBackgroundColourId, currentTheme.secondaryColour);
    addAndMakeVisible(outputSlider);

    outputLabel.setText("Output", juce::dontSendNotification);
    outputLabel.setFont(currentTheme.primaryFont);
    outputLabel.setColour(juce::Label::textColourId, currentTheme.textColour);
    outputLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(outputLabel);

    // Setup Mix slider
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    mixSlider.setRange(0.0, 1.0, 0.01);
    mixSlider.setValue(1.0);
    mixSlider.setColour(juce::Slider::rotarySliderFillColourId, currentTheme.primaryColour);
    mixSlider.setColour(juce::Slider::rotarySliderOutlineColourId, currentTheme.outlineColour);
    mixSlider.setColour(juce::Slider::textBoxTextColourId, currentTheme.textColour);
    mixSlider.setColour(juce::Slider::textBoxBackgroundColourId, currentTheme.secondaryColour);
    addAndMakeVisible(mixSlider);

    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setFont(currentTheme.primaryFont);
    mixLabel.setColour(juce::Label::textColourId, currentTheme.textColour);
    mixLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mixLabel);

    // Note: Parameter attachments would typically be created here if using AudioProcessorValueTreeState
    // For this example, we'll manually connect the sliders to parameters in the timer callback
}

void SimpleDistortionEditor::paintBackground(juce::Graphics& g)
{
    // Paint gradient background
    juce::ColourGradient gradient(
        currentTheme.backgroundColour.brighter(0.1f), 0, 0,
        currentTheme.backgroundColour.darker(0.2f), 0, static_cast<float>(getHeight()),
        false
    );
    
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());

    // Add subtle border
    g.setColour(currentTheme.outlineColour);
    g.drawRect(getLocalBounds(), 2);

    // Add some decorative elements
    g.setColour(currentTheme.primaryColour.withAlpha(0.1f));
    g.fillRoundedRectangle(20, 60, getWidth() - 40, getHeight() - 80, 10);
}

void SimpleDistortionEditor::layoutComponents()
{
    auto bounds = getLocalBounds();
    
    // Title area
    titleLabel.setBounds(bounds.removeFromTop(50).reduced(10));
    
    // Main control area
    auto controlArea = bounds.reduced(20);
    auto controlWidth = controlArea.getWidth() / 3;
    
    // Drive section
    auto driveArea = controlArea.removeFromLeft(controlWidth);
    driveLabel.setBounds(driveArea.removeFromTop(30));
    driveSlider.setBounds(driveArea.reduced(5));
    
    // Output section
    auto outputArea = controlArea.removeFromLeft(controlWidth);
    outputLabel.setBounds(outputArea.removeFromTop(30));
    outputSlider.setBounds(outputArea.reduced(5));
    
    // Mix section
    mixLabel.setBounds(controlArea.removeFromTop(30));
    mixSlider.setBounds(controlArea.reduced(5));
}