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
    addAndMakeVisible (timemarker);
    timemarker.setAlwaysOnTop (true);
}

TimeLine::~TimeLine()
{
    if (edit)
        edit->removeTimecodeListener (this);

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

    auto sampleRate = getSampleRate();
    if (sampleRate == 0)
        sampleRate = 48000.0;

    for (auto& clip : clipComponents)
        clip->setBounds (getXFromTime (clip->clip->start / sampleRate), 30,
                         getXFromTime (clip->clip->length / sampleRate), 80);

    auto time = player.getCurrentTimecode();
    auto t = getXFromTime (time.count * time.timebase);
    timemarker.setBounds (t, 0, 3, getHeight());
}

void TimeLine::timecodeChanged (foleys::Timecode time)
{
    auto t = getXFromTime (time.count * time.timebase);
    timemarker.setBounds (t, 0, 3, getHeight());
}

void TimeLine::mouseDown (const MouseEvent& event)
{
    player.setPosition (getTimeFromX (event.x));
}

bool TimeLine::isInterestedInFileDrag (const StringArray& files)
{
    return edit != nullptr;
}

void TimeLine::filesDropped (const StringArray& files, int x, int y)
{
    if (files.isEmpty() || edit == nullptr)
        return;

    addClipToEdit (files [0], getTimeFromX (x));
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
        addClipToEdit (source->getSelectedFile(), getTimeFromX (dragSourceDetails.localPosition.x));
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

    setSelectedClip (descriptor);
    resized();
}

void TimeLine::setSelectedClip (std::shared_ptr<foleys::AVCompoundClip::ClipDescriptor> clip)
{
    selectedClip = clip;
    repaint();
}

std::shared_ptr<foleys::AVCompoundClip::ClipDescriptor> TimeLine::getSelectedClip() const
{
    return selectedClip.lock();
}

void TimeLine::setEditClip (std::shared_ptr<foleys::AVCompoundClip> clip)
{
    if (edit)
        edit->removeTimecodeListener (this);

    edit = clip;

    if (edit)
        edit->addTimecodeListener (this);

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

double TimeLine::getSampleRate() const
{
    return player.getSampleRate();
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
    bool selected = timeline.getSelectedClip() == clip;
    g.fillAll (Colours::darkgrey);
    g.setColour (selected ? Colours::orange : Colours::orange.darker());
    g.fillRoundedRectangle (getLocalBounds().reduced (1).toFloat(), 5.0);

    g.setColour (selected ? Colours::darkorange.darker() : Colours::orange);

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
    timeline.setSelectedClip (clip);

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

    auto sampleRate = timeline.getSampleRate();

    if (dragmode == dragPosition)
        clip->start = std::max (timeline.getTimeFromX ((event.x - localDragStart.x) + getX()), 0.0) * sampleRate;
    else if (dragmode == dragLength)
        clip->length = timeline.getTimeFromX (event.x) * sampleRate;

    parent->resized();
}

void TimeLine::ClipComponent::mouseUp (const MouseEvent& event)
{
    dragmode = notDragging;
}
