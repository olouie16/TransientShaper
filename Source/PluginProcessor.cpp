/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TransientShaperAudioProcessor::TransientShaperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

TransientShaperAudioProcessor::~TransientShaperAudioProcessor()
{
}

//==============================================================================
const juce::String TransientShaperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TransientShaperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TransientShaperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TransientShaperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TransientShaperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TransientShaperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TransientShaperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TransientShaperAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TransientShaperAudioProcessor::getProgramName (int index)
{
    return {};
}

void TransientShaperAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TransientShaperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();


    float fastAttackInMs = 0.01;
    float slowAttackInMs = 1;
    float fastDecayInMs = 100;
    float slowDecayInMs = 100;


    fastAttackCoef = 1 - exp(-1 / (fastAttackInMs * sampleRate * 0.001));
    fastDecayCoef = 1 - exp(-1 / (fastDecayInMs * sampleRate * 0.001));
    slowAttackCoef = 1 - exp(-1 / (slowAttackInMs * sampleRate * 0.001));
    slowDecayCoef = 1 - exp(-1 / (slowDecayInMs * sampleRate * 0.001));


    fastEnvelope = std::vector<double>(totalNumInputChannels, 0);
    slowEnvelope = std::vector<double>(totalNumInputChannels, 0);

    attackFactor = 0;

}

void TransientShaperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TransientShaperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TransientShaperAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    // In case we have more outputs than inputs, this avoids garbage
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    double absSample;
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        for (int i = 0; i < buffer.getNumSamples(); i++) {

            absSample = abs(channelData[i]);
            fastEnvelope[channel] += (absSample - fastEnvelope[channel]) * (absSample > fastEnvelope[channel] ? fastAttackCoef : fastDecayCoef);
            slowEnvelope[channel] += (absSample - slowEnvelope[channel]) * (absSample > slowEnvelope[channel] ? slowAttackCoef : slowDecayCoef);


            if (fastEnvelope[channel] > 0 && slowEnvelope[channel] > 0) {

                channelData[i] *= pow(pow(fastEnvelope[channel] / slowEnvelope[channel] -1,2) +1, attackFactor);
            }
        }
    }


}


void TransientShaperAudioProcessor::updateAttackFactor(float factor) {

    attackFactor = factor;
}

//==============================================================================
bool TransientShaperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TransientShaperAudioProcessor::createEditor()
{
    return new TransientShaperAudioProcessorEditor (*this);
}

//==============================================================================
void TransientShaperAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TransientShaperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TransientShaperAudioProcessor();
}



