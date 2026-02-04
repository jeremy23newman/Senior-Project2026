/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GainExampleAudioProcessorEditor::GainExampleAudioProcessorEditor (GainExampleAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (200, 400);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    gainSlider.setRange(0.0, 1.0);
    gainSlider.setValue(.5);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
    
}

GainExampleAudioProcessorEditor::~GainExampleAudioProcessorEditor()
{
}

//==============================================================================
void GainExampleAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    }

void GainExampleAudioProcessorEditor::resized()
{
    gainSlider.setBounds(getLocalBounds());
}

void GainExampleAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &gainSlider) {
        
        audioProcessor.rawVolume = gainSlider.getValue();
        
    }
}
