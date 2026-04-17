/*
 ==============================================================================
 
 This file contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 */
class TekMixAudioProcessor  : public juce::AudioProcessor
{
    public:
    //==============================================================================
    TekMixAudioProcessor();
    ~TekMixAudioProcessor() override;
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const juce::String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::AudioProcessorValueTreeState apvts;
    
    
    std::vector<float> dnBuffer;
    float samplingRate = 44100.0f;
    
    
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    
    //EFFECT METHODS
    void processFilter(juce::AudioBuffer<float>& buffer);
    void processDelay(juce::AudioBuffer<float>& buffer);
    void processOverdrive(juce::AudioBuffer<float>& buffer);
    void processBitcrush(juce::AudioBuffer<float>& buffer);
    
    //DELAY HELPER METHODS
    void fillBuffer(juce::AudioBuffer<float>& buffer, int channel);
    void readFromBuffer(juce::AudioBuffer<float>& buffer,
                        juce::AudioBuffer<float>& delayBuffer,
                        int channel);
    void updateBufferPositions(juce::AudioBuffer<float>& buffer,
                               juce::AudioBuffer<float>& delayBuffer);
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TekMixAudioProcessor)
};
