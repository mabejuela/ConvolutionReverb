/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ConvolutionReverbAudioProcessorEditor::ConvolutionReverbAudioProcessorEditor (ConvolutionReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    for (auto* comp : getComps()) {
        addAndMakeVisible(comp);
    }
    
    setSize (600, 400);
}

ConvolutionReverbAudioProcessorEditor::~ConvolutionReverbAudioProcessorEditor()
{
}

//==============================================================================
void ConvolutionReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ConvolutionReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    auto paramArea = bounds.removeFromTop(bounds.getHeight() * 0.66);
    
    // getting half of paramArea for each parameter
    auto mixArea = paramArea.removeFromLeft(bounds.getWidth() * 0.5);
    auto gainArea = paramArea.removeFromRight(bounds.getWidth());
    
    gainSlider.setBounds(gainArea);
    mixSlider.setBounds(mixArea);
}

std::vector<juce::Component*> ConvolutionReverbAudioProcessorEditor::getComps()
{
    return {
        &gainSlider,
        &mixSlider
    };
}
