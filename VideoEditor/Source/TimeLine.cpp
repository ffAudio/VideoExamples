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
TimeLine::TimeLine (foleys::VideoEngine& theVideoEngine, Player& playerToUse)
  : videoEngine (theVideoEngine),
    player (playerToUse)
{
}

TimeLine::~TimeLine()
{
    edit = nullptr;
}

void TimeLine::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component
}

bool TimeLine::isInterestedInFileDrag (const StringArray& files)
{
    return edit != nullptr;
}

void TimeLine::filesDropped (const StringArray& files, int x, int y)
{
    if (files.isEmpty() || edit == nullptr)
        return;

    addClipToEdit (files [0], x * 10.0 / getWidth());
}

bool TimeLine::isInterestedInDragSource (const SourceDetails &dragSourceDetails)
{
    if (edit == nullptr)
        return false;

    return (dragSourceDetails.description == "media");
}

void TimeLine::itemDropped (const SourceDetails &dragSourceDetails)
{
    if (edit == nullptr)
        return;

    if (auto* source = dynamic_cast<FileTreeComponent*> (dragSourceDetails.sourceComponent.get()))
    {
        addClipToEdit (source->getSelectedFile(), dragSourceDetails.localPosition.x * 10.0 / getWidth());

        // TODO this is brutal testing only
//        auto* strip = new foleys::FilmStrip();
//        strip->setClip (clip);
//        strip->setStartAndLength (0, 20);
//        strip->setBounds (dragSourceDetails.localPosition.x, dragSourceDetails.localPosition.y, 1200, 80);
//        addAndMakeVisible (strip);
    }
}

void TimeLine::addClipToEdit (juce::File file, double start)
{
    auto length = -1.0;
    auto clip = videoEngine.createClipFromFile (file);
    if (std::dynamic_pointer_cast<foleys::AVMovieClip>(clip) == nullptr)
        length = 3.0;

    edit->addClip (clip, start, length);
}

void TimeLine::setEditClip (std::shared_ptr<foleys::AVCompoundClip> clip)
{
    edit = clip;
    player.setClip (edit);
}

std::shared_ptr<foleys::AVCompoundClip> TimeLine::getEditClip() const
{
    return edit;
}
