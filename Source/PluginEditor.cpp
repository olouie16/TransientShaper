/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TransientShaperAudioProcessorEditor::TransientShaperAudioProcessorEditor (TransientShaperAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);


    addAndMakeVisible(attackFactorSlider);
    attackFactorSlider.setRange(-2.0, 2.0);
    attackFactorSlider.onValueChange = [this] {audioProcessor.updateAttackFactor(attackFactorSlider.getValue()); };

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


}

