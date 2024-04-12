/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class TransientShaperAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    TransientShaperAudioProcessorEditor (TransientShaperAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~TransientShaperAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TransientShaperAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;


    juce::Slider attackFactorSlider;
    std::unique_ptr<SliderAttachment> attackFactorAttachment;
    juce::Label attackFactorLabel;

    juce::Slider releaseFactorSlider;
    std::unique_ptr<SliderAttachment> releaseFactorAttachment;
    juce::Label releaseFactorLabel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransientShaperAudioProcessorEditor)
};
