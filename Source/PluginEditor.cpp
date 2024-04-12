/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TransientShaperAudioProcessorEditor::TransientShaperAudioProcessorEditor (TransientShaperAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState (vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);


    addAndMakeVisible(attackFactorSlider);
    attackFactorSlider.setRange(-2.0, 2.0);
    //attackFactorSlider.onValueChange = [this] {audioProcessor.updateAttackFactor(attackFactorSlider.getValue()); };
    attackFactorAttachment.reset(new SliderAttachment(valueTreeState, "attackFactor", attackFactorSlider));

    addAndMakeVisible(releaseFactorSlider);
    releaseFactorSlider.setRange(-20.0, 20.0);
    //releaseFactorSlider.onValueChange = [this] {audioProcessor.updateReleaseFactor(releaseFactorSlider.getValue()); };
    releaseFactorAttachment.reset(new SliderAttachment(valueTreeState, "releaseFactor", releaseFactorSlider));

}

TransientShaperAudioProcessorEditor::~TransientShaperAudioProcessorEditor()
{
}

//==============================================================================
void TransientShaperAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void TransientShaperAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..


    float sliderLeft = 120;
    attackFactorSlider.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    releaseFactorSlider.setBounds(sliderLeft, 60, getWidth() - sliderLeft - 10, 20);


}

