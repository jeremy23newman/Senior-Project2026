/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TekMixAudioProcessorEditor::TekMixAudioProcessorEditor (TekMixAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p)
{
    
    setSize (900, 500);
    
    
    auto setupKnob = [](juce::Slider& s)
    {
        s.setSliderStyle(juce::Slider::Rotary);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    };
    
    setupKnob(driveKnob);
    setupKnob(bitDepthKnob);
    setupKnob(delayKnob);
    setupKnob(inputGainKnob);
    setupKnob(outputGainKnob);
    setupKnob(masterMixKnob);
    setupKnob(feedbackKnob);
    
    //KNOBS
    addAndMakeVisible(driveKnob);
    addAndMakeVisible(bitDepthKnob);
    addAndMakeVisible(delayKnob);
    addAndMakeVisible(inputGainKnob);
    addAndMakeVisible(outputGainKnob);
    addAndMakeVisible(masterMixKnob);
    addAndMakeVisible(feedbackKnob);
    
    
    
    //BUTTONS
    delayBypassButton.setButtonText("Delay Off");
    addAndMakeVisible(delayBypassButton);
    
    bitcrushBypassButton.setButtonText("Crush Off");
    addAndMakeVisible(bitcrushBypassButton);
    
    overdriveBypassButton.setButtonText("Drive Off");
    addAndMakeVisible(overdriveBypassButton);
    
    highpassButton.setButtonText("High Pass");
    addAndMakeVisible(highpassButton);
    
    //LABELS
    driveLabel.setText("Drive", juce::dontSendNotification);
    bitDepthLabel.setText("Bit Depth", juce::dontSendNotification);
    delayLabel.setText("Delay TIme", juce::dontSendNotification);
    inputLabel.setText("Input", juce::dontSendNotification);
    outputLabel.setText("Output", juce::dontSendNotification);
    feedbackLabel.setText("Delay Feedback", juce::dontSendNotification);
    masterMixLabel.setText("Mix", juce::dontSendNotification);
    delaySectionLabel.setText("Delay",
                              juce::dontSendNotification);
    
    
    //STYLING LABELS
    for (auto* label : { &driveLabel, &bitDepthLabel, &delayLabel,
        &inputLabel, &outputLabel,
        &feedbackLabel, &masterMixLabel,
        &delaySectionLabel })
    {
        label->setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    }
    
    juce::FontOptions options;
    options = options.withHeight(18.0f).withStyle("Bold");
    
    delaySectionLabel.setFont(juce::Font(options));
    
    //FILTER SLIDER
    cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(cutoffSlider);
    
    //ATTACHMENTS
    driveAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                         audioProcessor.apvts, "drive", driveKnob);
    
    bitDepthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                            audioProcessor.apvts, "bitDepth", bitDepthKnob);
    
    delayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                         audioProcessor.apvts, "delayTime", delayKnob);
    
    feedbackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                            audioProcessor.apvts, "feedback", feedbackKnob);
    
    cutoffAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                          audioProcessor.apvts, "cutoff", cutoffSlider);
    
    hpAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                                                                      audioProcessor.apvts, "highpass", highpassButton);
    
    inputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                         audioProcessor.apvts, "inputGain", inputGainKnob);
    
    outputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                          audioProcessor.apvts, "outputGain", outputGainKnob);
    
    mixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                                                                       audioProcessor.apvts, "masterMix", masterMixKnob);
    
    delayBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                                                                               audioProcessor.apvts, "bypassDelay", delayBypassButton);
    
    bitcrushBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                                                                                  audioProcessor.apvts, "bypassBitcrush", bitcrushBypassButton);
    
    overdriveBypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                                                                                   audioProcessor.apvts, "bypassOverdrive", overdriveBypassButton);
    
    
    
}

TekMixAudioProcessorEditor::~TekMixAudioProcessorEditor() {}

//==============================================================================
void TekMixAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    g.fillAll(juce::Colour(120, 140, 120));
    
    //TITLE
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(36.0f, juce::Font::bold));
    g.drawFittedText("TekMix", getLocalBounds().removeFromTop(60), juce::Justification::centred, 1);
}

void TekMixAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    
    const int labelH = 20;
    const int knobSize = 70;
    const int buttonH = 25;
    const int spacing = 8;
    
    
    area.removeFromTop(60);
    

    //MAIN GRID
    auto main = area.removeFromTop(300);
    
    int numCols = 6;
    int colW = main.getWidth() / numCols;
    
    auto getCol = [&](int i)
    {
        return juce::Rectangle<int>(
                                    main.getX() + i * colW,
                                    main.getY(),
                                    colW,
                                    main.getHeight()
                                    );
    };
    
    auto drawColumn = [&](juce::Rectangle<int> col,
                          juce::Slider& knob,
                          juce::Label& label,
                          juce::Button* button = nullptr)
    {
        col = col.reduced(10);
        
        label.setBounds(col.removeFromTop(labelH));
        
        auto knobArea = col.removeFromTop(knobSize);
        knob.setBounds(knobArea.withSizeKeepingCentre(knobSize, knobSize));
        
        if (button)
        {
            col.removeFromTop(spacing);
            button->setBounds(col.removeFromTop(buttonH).reduced(5));
        }
    };
    
    //INPUT
    drawColumn(getCol(0), inputGainKnob, inputLabel);
    
    //DRIVE
    drawColumn(getCol(1), driveKnob, driveLabel, &overdriveBypassButton);
    
    //CRUSH
    drawColumn(getCol(2), bitDepthKnob, bitDepthLabel, &bitcrushBypassButton);
    
    //DELAY
    auto delayCol = getCol(3);
    
    delaySectionLabel.setBounds(delayCol.removeFromTop(labelH));
    
    //TIME
    delayLabel.setBounds(delayCol.removeFromTop(labelH));
    auto d1 = delayCol.removeFromTop(knobSize);
    delayKnob.setBounds(d1.withSizeKeepingCentre(knobSize, knobSize));
    
    delayCol.removeFromTop(spacing);
    
    //FEEDBACK
    feedbackLabel.setBounds(delayCol.removeFromTop(labelH));
    auto d2 = delayCol.removeFromTop(knobSize);
    feedbackKnob.setBounds(d2.withSizeKeepingCentre(knobSize, knobSize));
    
    delayCol.removeFromTop(spacing);
    delayBypassButton.setBounds(delayCol.removeFromTop(buttonH).reduced(5));
    
    //MIX
    drawColumn(getCol(4), masterMixKnob, masterMixLabel);
    
    //OUTPUT
    drawColumn(getCol(5), outputGainKnob, outputLabel);
    
    //FILTER
    auto bottom = area.reduced(10);
    
    cutoffSlider.setBounds(bottom.removeFromTop(35));
    highpassButton.setBounds(bottom.removeFromTop(30));
}
