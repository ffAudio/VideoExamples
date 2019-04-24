/*
  ==============================================================================

    Properties.cpp
    Created: 30 Mar 2019 4:45:22pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Properties.h"

//==============================================================================

Properties::Properties()
{
    addAndMakeVisible (close);

    close.onClick = [&]
    {
        showProperties ({});
        repaint();
    };
}

void Properties::paint (Graphics& g)
{
    auto background = getLookAndFeel().findColour (ResizableWindow::backgroundColourId);
    g.fillAll (background);   // clear the background
    g.setColour (Colours::silver);
    g.drawFittedText (NEEDS_TRANS ("Properties"), getLocalBounds().withHeight (38).reduced (10, 3), Justification::left, 1);

    if (component)
    {
        g.setColour (background.darker());
        g.fillRect (getLocalBounds().withTop (40).reduced (5));
    }
}

void Properties::resized()
{
    close.setBounds (getLocalBounds().withHeight (38).reduced (5).removeFromRight (20));

    if (component)
        component->setBounds (getLocalBounds().withTop (40).reduced (5));
}

void Properties::showProperties (std::unique_ptr<Component> componentToDisplay)
{
    if (componentToDisplay.get() == nullptr)
    {
        component.reset();
        return;
    }

    component = std::move (componentToDisplay);
    addAndMakeVisible (component.get());
    component->setBounds (getLocalBounds().withTop (40).reduced (5));
}
