/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ConvolutionReverbAudioProcessor::ConvolutionReverbAudioProcessor()
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

ConvolutionReverbAudioProcessor::~ConvolutionReverbAudioProcessor()
{
}

//==============================================================================
const juce::String ConvolutionReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ConvolutionReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ConvolutionReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ConvolutionReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ConvolutionReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ConvolutionReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ConvolutionReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ConvolutionReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ConvolutionReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void ConvolutionReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ConvolutionReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    juce::dsp::ProcessSpec spec;
    
    // max number of samples it will process at one time
    spec.maximumBlockSize = samplesPerBlock;
    
    // number of channels
    spec.numChannels = 2;
    
    // sample rate
    spec.sampleRate = sampleRate;
    
    gain.prepare(spec);
    conv.prepare(spec);
    mix.prepare(spec);
    
    auto wetLatency = conv.getLatency();
    mix.setWetLatency(wetLatency);
    
//    gain.setGainLinear(1.0f);
//    gain.setGainDecibels(-24.0f);
    
    updateParameters();
    
    // Convolution
    auto file = juce::File("/Users/mabejuela/Downloads/EchoThief/Domes/Kroc Rotunda University of San Diego California.wav");
    juce::dsp::Convolution::Stereo convStereo = juce::dsp::Convolution::Stereo::yes;
    juce::dsp::Convolution::Trim convTrim = juce::dsp::Convolution::Trim::no;
    
    conv.loadImpulseResponse(file, convStereo, convTrim, 0);
    
}

void ConvolutionReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ConvolutionReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ConvolutionReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        // ..do something to the data...
    }
    
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    updateParameters();
    
    mix.pushDrySamples(block);

    conv.process(context);
    
    mix.mixWetSamples(block);
    
    gain.process(context);
}

//==============================================================================
bool ConvolutionReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ConvolutionReverbAudioProcessor::createEditor()
{
    return new ConvolutionReverbAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ConvolutionReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void ConvolutionReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if ( tree.isValid() ) {
        apvts.replaceState(tree);
        updateParameters();
    }
}

//void ConvolutionReverbAudioProcessor::updateParameter(juce::AudioProcessorValueTreeState& apvts, juce::dsp::AudioBlock<float> block)
//{
////    //    updateParameter();
//        ChainSettings chainSettings;
////        
////    //    \/ same as == chainSettings.mixDryWet = (*apvts.getRawParameterValue("Dry/Wet")).load();
//        chainSettings.mixDryWet = apvts.getRawParameterValue("Dry/Wet")->load();
//    
//    float mixValue = apvts.getRawParameterValue("Dry/Wet")->load();
//    
//    juce::dsp::DryWetMixer<float> setWetMixProportion(mixValue);
////    juce::dsp::DryWetMixer< float >::pushDrySamples    (    block    )    ;
////    juce::dsp::DryWetMixer<float>::pushDrySamples(block);
////    chainSettings.pushDrySamples(block);
//    
//}

void ConvolutionReverbAudioProcessor::updateParameters()
{
    float gainValue = apvts.getRawParameterValue("Gain")->load();
    gain.setGainDecibels(gainValue);
    
    float mixValue = apvts.getRawParameterValue("Dry/Wet")->load();
    mix.setWetMixProportion(mixValue);
}

juce::AudioProcessorValueTreeState::ParameterLayout ConvolutionReverbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"Dry/Wet", 1},
                                                           "Dry/Wet",
                                                           juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f), // change from 0-1 into 0-100%
                                                           0.55f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"Gain", 1},
                                                           "Gain",
                                                           juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                           0.f));
    
//    layout.add(std::make_unique<juce::AudioParameterFloat(juce::ParameterID(<#const ParameterID &#>), <#const String &parameterName#>, <#NormalisableRange<float> normalisableRange#>, <#float defaultValue#>)
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ConvolutionReverbAudioProcessor();
}
