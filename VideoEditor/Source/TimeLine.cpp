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
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker());
}

void TimeLine::resized()
{
    if (edit == nullptr)
        return;

    auto sampleRate = player.getSampleRate();
    if (sampleRate == 0)
        sampleRate = 48000.0;

    for (auto& clip : clipComponents)
        clip->setBounds (getXFromTime (clip->clip->start / sampleRate), 30,
                         getXFromTime (clip->clip->length / sampleRate), 80);
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
    }
}

void TimeLine::addClipToEdit (juce::File file, double start)
{
    auto length = -1.0;
    auto clip = videoEngine.createClipFromFile (file);
    if (std::dynamic_pointer_cast<foleys::AVMovieClip>(clip) == nullptr)
        length = 3.0;

    auto descriptor = edit->addClip (clip, start, length);

    // TODO this is brutal testing only
    auto strip = std::make_unique<ClipComponent> (*this, descriptor, videoEngine.getThreadPool());
    addAndMakeVisible (strip.get());
    clipComponents.emplace_back (std::move (strip));

    resized();
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

int TimeLine::getXFromTime (double seconds) const
{
    return (seconds / 120.0) * getWidth();
}

double TimeLine::getTimeFromX (int pixels) const
{
    auto w = getWidth();
    return w > 0 ? 120.0 * pixels / w : 0;
}

//==============================================================================

TimeLine::ClipComponent::ClipComponent (TimeLine& tl,
                                        std::shared_ptr<foleys::AVCompoundClip::ClipDescriptor> clipToUse,
                                        ThreadPool& threadPool)
  : clip (clipToUse),
    timeline (tl),
    filmstrip (threadPool)
{
    filmstrip.setClip (clip->clip);
    addAndMakeVisible (filmstrip);
}

void TimeLine::ClipComponent::paint (Graphics& g)
{
    g.fillAll (Colours::darkgrey);
    g.setColour (Colours::orange.darker());
    g.fillRoundedRectangle (getLocalBounds().reduced (1).toFloat(), 5.0);
    g.setColour (Colours::orange);
    g.drawRoundedRectangle (getLocalBounds().toFloat(), 5.0, 2.0);
    if (clip != nullptr)
        g.drawFittedText (clip->name, 5, 3, getWidth() - 10, 18, Justification::left, 1);
}

void TimeLine::ClipComponent::resized()
{
    filmstrip.setBounds (1, 20, getWidth() - 2, getHeight() - 25);
    filmstrip.setStartAndLength (timeline.getTimeFromX (getX()), timeline.getTimeFromX (getWidth()));
}

void TimeLine::ClipComponent::mouseMove (const MouseEvent& event)
{
    if (event.x > getWidth() - 5)
        setMouseCursor (MouseCursor::LeftRightResizeCursor);
    else
        setMouseCursor (MouseCursor::DraggingHandCursor);

}

void TimeLine::ClipComponent::mouseDown (const MouseEvent& event)
{
    localDragStart = event.getPosition();

    if (event.x > getWidth() - 5)
        dragmode = dragLength;
    else
        dragmode = dragPosition;
}

void TimeLine::ClipComponent::mouseDrag (const MouseEvent& event)
{
    auto* parent = getParentComponent();
    if (parent == nullptr)
        return;

    if (dragmode == dragPosition)
        clip->start = timeline.getTimeFromX ((event.x - localDragStart.x) + getX()) * 44100;
    else if (dragmode == dragLength)
        clip->length = timeline.getTimeFromX (event.x) * 44100;

    parent->resized();
}

void TimeLine::ClipComponent::mouseUp (const MouseEvent& event)
{
    dragmode = notDragging;
}
