/*
  ==============================================================================

    TimeLine.cpp
    Created: 30 Mar 2019 4:46:00pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "Player.h"
#include "TimeLine.h"

//==============================================================================
TimeLine::TimeLine (Player& playerToUse)
  : player (playerToUse)
{
}

TimeLine::~TimeLine()
{
}

void TimeLine::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

bool TimeLine::isInterestedInFileDrag (const StringArray& files)
{
    return true;
}

void TimeLine::filesDropped (const StringArray& files, int x, int y)
{
    if (files.isEmpty())
        return;

    auto clip = foleys::AVFormatManager::createClipFromFile (files [0]);
    player.setClip (clip);
}

bool TimeLine::isInterestedInDragSource (const SourceDetails &dragSourceDetails)
{
    return (dragSourceDetails.description == "media");
}

void TimeLine::itemDropped (const SourceDetails &dragSourceDetails)
{
    if (auto* source = dynamic_cast<FileTreeComponent*> (dragSourceDetails.sourceComponent.get()))
    {
        auto clip = foleys::AVFormatManager::createClipFromFile (source->getSelectedFile());
        foleys::VideoEngine::getInstance()->addClip (clip);
        player.setClip (clip);

        // TODO this is brutal testing only
        auto* strip = new foleys::FilmStrip();
        strip->setClip (clip);
        strip->setStartAndLength (0, 20);
        strip->setBounds (dragSourceDetails.localPosition.x, dragSourceDetails.localPosition.y, 1200, 80);
        addAndMakeVisible (strip);
    }
}
