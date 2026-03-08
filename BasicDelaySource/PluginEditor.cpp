/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CircularDelayBufferAudioProcessorEditor::CircularDelayBufferAudioProcessorEditor (CircularDelayBufferAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
 
    setSize (400, 300);

        delaySlider.setSliderStyle(juce::Slider::Rotary);
        delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(delaySlider);

        delayAttachment = std::make_unique<
                juce::AudioProcessorValueTreeState::SliderAttachment>(
                    audioProcessor.apvts,
                    "delayTime",
                    delaySlider);

        wetDrySlider.setSliderStyle(juce::Slider::Rotary);
        wetDrySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(wetDrySlider);

        wetDryAttachment = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
                audioProcessor.apvts,
                "wetDry",
                wetDrySlider);


        delayLabel.setText("Delay Time", juce::dontSendNotification);
        delayLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(delayLabel);

        wetDryLabel.setText("Wet/Dry", juce::dontSendNotification);
        wetDryLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(wetDryLabel);
}

CircularDelayBufferAudioProcessorEditor::~CircularDelayBufferAudioProcessorEditor()
{
}

//==============================================================================
void CircularDelayBufferAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void CircularDelayBufferAudioProcessorEditor::resized()
{
    delaySlider.setBounds(50, 50, 150, 150);
    wetDrySlider.setBounds(220, 50, 150, 150);
    delayLabel.setBounds(50, 30, 150, 20);
    wetDryLabel.setBounds(220, 30, 150, 20);
}


