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
    //int totalNumOutputChannels = getTotalNumOutputChannels();


    float attackInS = 0.00001;
    float releaseInS = 0.100;

    float slowAttackInS = 0.001;
    float slowReleaseInS = 0.250;



    fastAttackCoefAtt = 1 - exp(-1 / (attackInS * sampleRate));
    fastReleaseCoefAtt = 1 - exp(-1 / (releaseInS * sampleRate));
    slowAttackCoefAtt = 1 - exp(-1 / (slowAttackInS * sampleRate));
    slowReleaseCoefAtt = 1 - exp(-1 / (releaseInS * sampleRate));

    fastAttackCoefRel = 1 - exp(-1 / (attackInS * sampleRate));
    fastReleaseCoefRel = 1 - exp(-1 / (releaseInS * sampleRate));
    slowAttackCoefRel = 1 - exp(-1 / (attackInS * sampleRate));
    slowReleaseCoefRel = 1 - exp(-1 / (slowReleaseInS * sampleRate));


    fastEnvelopeAtt = std::vector<double>(totalNumInputChannels, 0);
    slowEnvelopeAtt = std::vector<double>(totalNumInputChannels, 0);
    fastEnvelopeRel = std::vector<double>(totalNumInputChannels, 0);
    slowEnvelopeRel = std::vector<double>(totalNumInputChannels, 0);

    attackFactor = 0;
    releaseFactor = 0;

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
            fastEnvelopeAtt[channel] += (absSample - fastEnvelopeAtt[channel]) * (absSample > fastEnvelopeAtt[channel] ? fastAttackCoefAtt : fastReleaseCoefAtt);
            slowEnvelopeAtt[channel] += (absSample - slowEnvelopeAtt[channel]) * (absSample > slowEnvelopeAtt[channel] ? slowAttackCoefAtt : slowReleaseCoefAtt);
            fastEnvelopeRel[channel] += (absSample - fastEnvelopeRel[channel]) * (absSample > fastEnvelopeRel[channel] ? fastAttackCoefRel : fastReleaseCoefRel);
            slowEnvelopeRel[channel] += (absSample - slowEnvelopeRel[channel]) * (absSample > slowEnvelopeRel[channel] ? slowAttackCoefRel : slowReleaseCoefRel);


            if (fastEnvelopeAtt[channel] > 0 && slowEnvelopeAtt[channel] > 0) {


                float a = fastEnvelopeAtt[channel] / slowEnvelopeAtt[channel]; //high while attack, else near 1
                a = pow(a - 1, 2) + 1; //get non attack values closer to 1, while boosting higher attack values
                a = pow(a, attackFactor); //userinput, q^0=1 -> no change, q^+x>1 and q^-x<1 -> positive input boosts and negative values decreases volume

                channelData[i] *= a;
            }

            if (fastEnvelopeRel[channel] > 0 && slowEnvelopeRel[channel] > 0) {


                float r = fastEnvelopeRel[channel] / slowEnvelopeRel[channel]; //high while attack, else near 1
                r = pow(r - 1, 2) + 1; //get non attack values closer to 1, while boosting higher attack values
                r = pow(r, releaseFactor); //userinput, q^0=1 -> no change, q^+x>1 and q^-x<1 -> positive input boosts and negative values decreases volume

                channelData[i] *= r;
            }

        }
    }


}


void TransientShaperAudioProcessor::updateAttackFactor(float factor) {

    attackFactor = factor;
}

void TransientShaperAudioProcessor::updateReleaseFactor(float factor) {

    releaseFactor = factor;
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



