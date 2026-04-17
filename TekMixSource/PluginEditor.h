#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class TekMixAudioProcessorEditor  : public juce::AudioProcessorEditor
{
    public:
    TekMixAudioProcessorEditor (TekMixAudioProcessor&);
    ~TekMixAudioProcessorEditor() override;
    
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // KNOBS
    juce::Slider driveKnob, bitDepthKnob, delayKnob;
    juce::Slider inputGainKnob, outputGainKnob, masterMixKnob, feedbackKnob;
    
    //LABELS
    juce::Label driveLabel, bitDepthLabel, delayLabel;
    juce::Label inputLabel, outputLabel, feedbackLabel, masterMixLabel, delaySectionLabel;
    
    //FILTER(SLIDER)
    juce::Slider cutoffSlider;
    juce::ToggleButton highpassButton;
    
    //BYPASS(BUTTONS)
    juce::ToggleButton delayBypassButton;
    juce::ToggleButton bitcrushBypassButton;
    juce::ToggleButton overdriveBypassButton;
    
    //ATTACHMENT
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitDepthAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> hpAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> delayBypassAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bitcrushBypassAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> overdriveBypassAttach;
    
    private:
    TekMixAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TekMixAudioProcessorEditor)
};
