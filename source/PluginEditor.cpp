#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    setupSlider (roomSizeSlider, roomSizeLabel, "Room Size");
    setupSlider (dampingSlider, dampingLabel, "Damping");
    setupSlider (wetLevelSlider, wetLevelLabel, "Wet");
    setupSlider (dryLevelSlider, dryLevelLabel, "Dry");
    setupSlider (widthSlider, widthLabel, "Width");

    addAndMakeVisible (freezeButton);

    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "roomSize", roomSizeSlider);
    dampingAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "damping", dampingSlider);
    wetLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "wetLevel", wetLevelSlider);
    dryLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "dryLevel", dryLevelSlider);
    widthAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (processorRef.apvts, "width", widthSlider);
    freezeAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processorRef.apvts, "freeze", freezeButton);

    addAndMakeVisible (inspectButton);
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }
        inspector->setVisible (true);
    };

    setSize (500, 350);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::setupSlider (juce::Slider& slider, juce::Label& label, const juce::String& labelText)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 18);
    addAndMakeVisible (slider);

    label.setText (labelText, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.attachToComponent (&slider, false);
    addAndMakeVisible (label);
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    g.drawText (PRODUCT_NAME_WITHOUT_VERSION, getLocalBounds().removeFromTop (40), juce::Justification::centred, false);
}

void PluginEditor::resized()
{
    auto area = getLocalBounds().reduced (10);

    // Header space (title + label row)
    area.removeFromTop (55);

    // Knob row
    auto knobArea = area.removeFromTop (130);
    auto knobWidth = knobArea.getWidth() / 5;

    roomSizeSlider.setBounds (knobArea.removeFromLeft (knobWidth));
    dampingSlider.setBounds (knobArea.removeFromLeft (knobWidth));
    wetLevelSlider.setBounds (knobArea.removeFromLeft (knobWidth));
    dryLevelSlider.setBounds (knobArea.removeFromLeft (knobWidth));
    widthSlider.setBounds (knobArea);

    // Bottom row
    area.removeFromTop (20);
    auto bottomArea = area.removeFromTop (30);
    freezeButton.setBounds (bottomArea.removeFromLeft (120));
    bottomArea.removeFromLeft (10);
    inspectButton.setBounds (bottomArea.removeFromLeft (120));
}
