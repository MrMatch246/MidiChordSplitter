#pragma once

#include "PluginProcessor.h"

//==============================================================================
class MidiChordSplitterAudioProcessorEditor final : public juce::AudioProcessorEditor , public juce::Button::Listener , public juce::ComboBox::Listener
{
public:
    explicit MidiChordSplitterAudioProcessorEditor(MidiChordSplitterAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~MidiChordSplitterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
private:

    // void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void updateLabels();
    void setButtonColors(juce::TextButton& button);

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MidiChordSplitterAudioProcessor& processorRef;
    juce::Label versionLabel;


    // New controls for high, mid, and low frequencies
    juce::TextButton highToggle;
    juce::TextButton midToggle;
    juce::TextButton lowToggle;

    juce::Slider highKnob;
    juce::Slider midKnob;
    juce::Slider lowKnob;

    juce::Label divLabel;

    juce::AudioProcessorValueTreeState& valueTreeState;

    // New attachments for high, mid, low toggles and knobs
    std::unique_ptr<ButtonAttachment> highToggleAttachment;
    std::unique_ptr<ButtonAttachment> midToggleAttachment;
    std::unique_ptr<ButtonAttachment> lowToggleAttachment;

    std::unique_ptr<SliderAttachment> highKnobAttachment;
    std::unique_ptr<SliderAttachment> midKnobAttachment;
    std::unique_ptr<SliderAttachment> lowKnobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiChordSplitterAudioProcessorEditor)
};
