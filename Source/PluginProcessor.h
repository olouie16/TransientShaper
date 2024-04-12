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
class TransientShaperAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TransientShaperAudioProcessor();
    ~TransientShaperAudioProcessor() override;

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

    void updateAttackFactor(float factor);
    void updateReleaseFactor(float factor);

private:
    //==============================================================================

    double fastAttackCoefAtt;
    double fastReleaseCoefAtt;
    double slowAttackCoefAtt;
    double slowReleaseCoefAtt;

    double fastAttackCoefRel;
    double fastReleaseCoefRel;
    double slowAttackCoefRel;
    double slowReleaseCoefRel;

    std::vector< double> fastEnvelopeAtt;
    std::vector< double> slowEnvelopeAtt;

    std::vector< double> fastEnvelopeRel;
    std::vector< double> slowEnvelopeRel;

    float attackFactor;
    float releaseFactor;





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransientShaperAudioProcessor)
};
