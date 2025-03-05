#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiChordSplitterAudioProcessorEditor::MidiChordSplitterAudioProcessorEditor(MidiChordSplitterAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p), processorRef(p), valueTreeState (vts)
{


    addAndMakeVisible(versionLabel);
    versionLabel.setText("Version: 1.2.1 ", juce::dontSendNotification);

    // High Frequency Toggle and Knob
    addAndMakeVisible(highToggle);
    highToggle.setClickingTogglesState(true);
    highToggle.setButtonText("High");
    setButtonColors(highToggle);
    highToggle.addListener(this);
    highToggleAttachment.reset(new ButtonAttachment(valueTreeState, "highToggle", highToggle));

    addAndMakeVisible(highKnob);
    highKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    highKnob.setRange(0.0f, 16.0f, 1.00f);
    highKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    highKnob.setTextValueSuffix(" Offset");
    highKnob.setValue(0.0f);
    highKnobAttachment.reset(new SliderAttachment(valueTreeState, "highKnob", highKnob));

    // Mid Frequency Toggle and Knob
    addAndMakeVisible(midToggle);
    midToggle.setClickingTogglesState(true);
    midToggle.setButtonText("Mid");
    setButtonColors(midToggle);
    midToggle.addListener(this);
    midToggleAttachment.reset(new ButtonAttachment(valueTreeState, "midToggle", midToggle));

    addAndMakeVisible(midKnob);
    midKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    midKnob.setRange(1.0f, 16.0f, 1.00f);
    midKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    midKnob.setTextValueSuffix(" Offset");
    midKnob.setValue(1.0f);
    midKnobAttachment.reset(new SliderAttachment(valueTreeState, "midKnob", midKnob));

    // Low Frequency Toggle and Knob
    addAndMakeVisible(lowToggle);
    lowToggle.setClickingTogglesState(true);
    lowToggle.setButtonText("Low");
    setButtonColors(lowToggle);
    lowToggle.addListener(this);
    lowToggleAttachment.reset(new ButtonAttachment(valueTreeState, "lowToggle", lowToggle));

    addAndMakeVisible(lowKnob);
    lowKnob.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lowKnob.setRange(0.0f, 16.0f, 1.00f);
    lowKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    lowKnob.setTextValueSuffix(" Offset");
    lowKnob.setValue(0.0f);
    lowKnobAttachment.reset(new SliderAttachment(valueTreeState, "lowKnob", lowKnob));

    setSize(200, 400);
}

MidiChordSplitterAudioProcessorEditor::~MidiChordSplitterAudioProcessorEditor()
{
}

//==============================================================================
void MidiChordSplitterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MidiChordSplitterAudioProcessorEditor::resized()
{
    // Get the available area
    auto area = getLocalBounds().reduced(10); // Leave some space for borders

    // We will use a grid-like layout for the toggles and knobs
    int padding = 3;  // Set some padding between components
    int toggleSize = 80;
    int knobSize = 90;

    // Define the positions for the toggles and knobs
    auto highToggleArea = area.removeFromTop(100);  // Make room for toggle
    highToggle.setBounds(highToggleArea.removeFromLeft(toggleSize).withHeight(toggleSize));
    highKnob.setBounds(highToggleArea.removeFromRight(knobSize).withHeight(knobSize));

    // Move the remaining area down for the next row
    area.removeFromTop(padding);  // Space between rows
    auto midToggleArea = area.removeFromTop(100);  // Make room for toggle
    midToggle.setBounds(midToggleArea.removeFromLeft(toggleSize).withHeight(toggleSize));
    midKnob.setBounds(midToggleArea.removeFromRight(knobSize).withHeight(knobSize));

    // Move the remaining area down for the next row
    area.removeFromTop(padding);  // Space between rows
    auto lowToggleArea = area.removeFromTop(100);  // Make room for toggle
    lowToggle.setBounds(lowToggleArea.removeFromLeft(toggleSize).withHeight(toggleSize));
    lowKnob.setBounds(lowToggleArea.removeFromRight(knobSize).withHeight(knobSize));

    // Version label at the bottom
    versionLabel.setBounds(area.removeFromBottom(30).reduced(0, 5));  // Leave some space for margin
}





void MidiChordSplitterAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &highToggle)
    {
        // Toggle button state has changed, handle the action here
        bool highEnabled = highToggle.getToggleState();
        highKnob.setEnabled(highEnabled);
    }
    else if (button == &midToggle)
    {
        // Toggle button state has changed, handle the action here
        bool midEnabled = midToggle.getToggleState();
        midKnob.setEnabled(midEnabled);
    }
    else if (button == &lowToggle)
    {
        // Toggle button state has changed, handle the action here
        bool lowEnabled = lowToggle.getToggleState();
        lowKnob.setEnabled(lowEnabled);
    }


    this->updateLabels();
}

void MidiChordSplitterAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBox)
{
    this->updateLabels();
}

void MidiChordSplitterAudioProcessorEditor::updateLabels()
{
}

void MidiChordSplitterAudioProcessorEditor::setButtonColors(juce::TextButton& button)
{
    button.setColour(juce::TextButton::buttonColourId, juce::Colours::turquoise);
    button.setColour(juce::TextButton::buttonOnColourId, juce::Colours::orange);
    button.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    button.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
}
