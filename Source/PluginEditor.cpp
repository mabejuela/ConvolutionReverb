/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics & g,
                                   int x,
                                   int y,
                                   int width,
                                   int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider & slider)
{
    using namespace juce;
    
    auto bounds = Rectangle<float>(x, y, width, height);
    
    g.setColour(Colour(97u, 18u, 167u));
    g.fillEllipse(bounds);
    
    g.setColour(Colour(255u, 154u, 1u));
    g.drawEllipse(bounds, 1.f);
    
    if ( auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider)) {
        auto center = bounds.getCentre();
        
        Path p;
        
        Rectangle<float> r;
        r.setLeft(center.getX() - 2);
        r.setRight(center.getX() + 2);
        r.setTop(bounds.getY());
        r.setBottom(center.getY() - rswl->getTextHeight() * 1.5);
        
        p.addRoundedRectangle(r, 2.f);
//        p.addRectangle(r); // if you want to rotate something that is going to be drawn, you need a path

        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        g.setFont(rswl->getTextHeight());
        auto text = rswl->getDisplayString();
        auto strWidth = g.getCurrentFont().getStringWidth(text);
        
        r.setSize(strWidth + 4, rswl->getTextHeight() + 2);
        r.setCentre(bounds.getCentre());
        
        g.setColour(Colours::black);
        g.fillRect(r);
        
        g.setColour(Colours::white);
        g.drawFittedText(text, r.toNearestInt(), juce::Justification::centred, 1);
    }
}

//==============================================================================
void RotarySliderWithLabels::paint(juce::Graphics &g)
{
    using namespace juce;
    
    auto startAng = degreesToRadians(180.f + 45.f);
    auto endAng = degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();
    
//    g.setColour(Colours::red);
//    g.drawRect(getLocalBounds());
//    g.setColour(Colours::yellow);
//    g.drawRect(sliderBounds);
    
    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    auto bounds = getLocalBounds();
    
    auto size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    
    size -= getTextHeight() * 2;
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), bounds.getCentreY());
//    r.setY(200);
    
    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    juce::String str;
    
    if ( auto* floatParam = dynamic_cast<juce::AudioParameterFloat*>(param)) {
        if (param->getName(200) == "Dry/Wet") {
            str = juce::String(getValue() * 100);
            str << suffix;
        }
        else{
            str = juce::String(getValue());
            str << " " << suffix;
        }
    }
    else{
        jassertfalse;
    }
    
    return str;
}

//==============================================================================
ConvolutionReverbAudioProcessorEditor::ConvolutionReverbAudioProcessorEditor (ConvolutionReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
gainSlider(*audioProcessor.apvts.getParameter("Gain"), "dB"),
mixSlider(*audioProcessor.apvts.getParameter("Dry/Wet"), "%"),
gainSliderAttachment(audioProcessor.apvts, "Gain", gainSlider),
mixSliderAttachment(audioProcessor.apvts, "Dry/Wet", mixSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    gainSlider.labels.add({0.f, "20Hz"});
    gainSlider.labels.add({1.f, "20kHz"});
    
    mixSlider.labels.add({0.f, "-24dB"});
    mixSlider.labels.add({1.f, "24dB"});
    
    fileSelectButton.setButtonText("Select the impulse response you want to load... (.wav file)");
    fileSelectButton.changeWidthToFitText();
    
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
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    using namespace juce;
    
    g.fillAll(Colours::black);
    
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);
    
    auto w = responseArea.getWidth();
    
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    
//    auto bounds = getLocalBounds();
//    auto paramArea = bounds.removeFromTop(bounds.getHeight() * 0.66);
//    
//    // getting half of paramArea for each parameter
//    auto mixArea = paramArea.removeFromLeft(bounds.getWidth() * 0.5);
//    auto gainArea = paramArea.removeFromRight(bounds.getWidth());
//    
//    g.drawFittedText("Dry/Wet", mixArea, juce::Justification::centredBottom, 1);
//    g.drawFittedText("Gain", gainArea, juce::Justification::centredBottom, 1);
}

void ConvolutionReverbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    auto paramArea = bounds.removeFromTop(bounds.getHeight() * 0.66);
    auto bottomBit = bounds.removeFromBottom(bounds.getHeight() * 0.4);
    auto buttonArea = bounds.removeFromBottom(bounds.getHeight() * 0.33);
//    auto buttonArea = buttonArea.removeFromTop(bounds.getHeight() * 0.66);
    
    // getting half of paramArea for each parameter
    auto mixArea = paramArea.removeFromLeft(bounds.getWidth() * 0.5);
    auto gainArea = paramArea.removeFromRight(bounds.getWidth());
    
    gainSlider.setBounds(gainArea);
    mixSlider.setBounds(mixArea);
    fileSelectButton.setBounds(buttonArea);
}

void ConvolutionReverbAudioProcessorEditor::loadMyFile()
{
    myChooser = std::make_unique<juce::FileChooser> ("Select the impulse response you want to load... (.wav file)",
                                               juce::File::getSpecialLocation (juce::File::userHomeDirectory),
                                               "*.wav");

    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    myChooser->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
    {
        juce::File myFile(chooser.getResult());
        
        if (myFile.existsAsFile()) {
            audioProcessor.loadMyImpulseResponse(myFile);
        }
    });
}

std::vector<juce::Component*> ConvolutionReverbAudioProcessorEditor::getComps()
{
    return {
        &gainSlider,
        &mixSlider,
        &fileSelectButton
    };
}
