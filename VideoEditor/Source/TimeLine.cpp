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
    {
        edit->removeTimecodeListener (this);
        edit->getStatusTree().removeListener (this);
    }

    edit = nullptr;
}

void TimeLine::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId).darker());

    g.setColour (Colours::darkgrey);
    for (int i=0; i < 2; ++i)
        g.fillRect (0, 30 + i * 90, getWidth(), 80);
}

void TimeLine::resized()
{
    if (edit == nullptr)
        return;

    auto sampleRate = getSampleRate();
    if (sampleRate == 0)
        sampleRate = 48000.0;

    for (auto& component : clipComponents)
    {
        auto videoline = component->clip->getVideoLine();

        component->setBounds (getXFromTime (component->clip->getStart()), 30 + videoline * 90,
                              getXFromTime (component->clip->getLength()), 80);
    }

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

    int line = (y - 30) / 90.0;
    addClipToEdit (files [0], getTimeFromX (x), line);
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
        int line = (dragSourceDetails.localPosition.y - 30) / 90.0;
        addClipToEdit (source->getSelectedFile(), getTimeFromX (dragSourceDetails.localPosition.x), line);
    }
}

void TimeLine::addClipToEdit (juce::File file, double start, int line)
{
    auto length = -1.0;
    auto clip = videoEngine.createClipFromFile (file);
    if (std::dynamic_pointer_cast<foleys::MovieClip>(clip) == nullptr)
        length = 3.0;

    auto descriptor = edit->addClip (clip, start, length);
    descriptor->setVideoLine (line);

    restoreClipComponents();

    setSelectedClip (descriptor);
    resized();
}

void TimeLine::setSelectedClip (std::shared_ptr<foleys::ComposedClip::ClipDescriptor> clip)
{
    selectedClip = clip;
    repaint();
}

std::shared_ptr<foleys::ComposedClip::ClipDescriptor> TimeLine::getSelectedClip() const
{
    return selectedClip.lock();
}

void TimeLine::restoreClipComponents()
{
    if (edit == nullptr)
        return;

    auto clips = edit->getClips();
    for (auto it = clips.begin(); it != clips.end(); ++it)
    {
        auto comp = std::find_if (clipComponents.begin(), clipComponents.end(), [it](const auto& comp){ return comp->clip == *it; });
        if (comp == clipComponents.end())
        {
            auto strip = std::make_unique<ClipComponent> (*this, *it, videoEngine.getThreadPool());
            addAndMakeVisible (strip.get());
            clipComponents.emplace_back (std::move (strip));
        }
    }
    for (auto it = clipComponents.begin(); it != clipComponents.end();)
    {
        auto clip = std::find_if (clips.begin(), clips.end(), [it](const auto& clip){ return (*it)->clip == clip; });
        if (clip == clips.end())
            it = clipComponents.erase (it);
        else
            ++it;
    }

    resized();
}

void TimeLine::setEditClip (std::shared_ptr<foleys::ComposedClip> clip)
{
    if (edit)
    {
        edit->removeTimecodeListener (this);
        edit->getStatusTree().removeListener (this);
    }

    edit = clip;

    if (edit)
    {
        edit->getStatusTree().addListener (this);
        edit->addTimecodeListener (this);
    }

    restoreClipComponents();

    player.setClip (edit);
}

std::shared_ptr<foleys::ComposedClip> TimeLine::getEditClip() const
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

void TimeLine::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                         const juce::Identifier& property)
{
    resized();
}

void TimeLine::valueTreeChildAdded (juce::ValueTree& parentTree,
                                    juce::ValueTree& childWhichHasBeenAdded)
{
    restoreClipComponents();
}

void TimeLine::valueTreeChildRemoved (juce::ValueTree& parentTree,
                                      juce::ValueTree& childWhichHasBeenRemoved,
                                      int indexFromWhichChildWasRemoved)
{
    restoreClipComponents();
}

//==============================================================================

TimeLine::ClipComponent::ClipComponent (TimeLine& tl,
                                        std::shared_ptr<foleys::ComposedClip::ClipDescriptor> clipToUse,
                                        ThreadPool& threadPool)
  : clip (clipToUse),
    timeline (tl)
{
    filmstrip = std::make_unique<foleys::FilmStrip>();
    filmstrip->setClip (clip->clip);
    addAndMakeVisible (filmstrip.get());
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
        g.drawFittedText (clip->getDescription(), 5, 3, getWidth() - 10, 18, Justification::left, 1);
}

void TimeLine::ClipComponent::resized()
{
    if (filmstrip)
    {
        filmstrip->setBounds (1, 20, getWidth() - 2, getHeight() - 25);
        filmstrip->setStartAndLength (timeline.getTimeFromX (getX()), timeline.getTimeFromX (getWidth()));
    }
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

    if (dragmode == dragPosition)
        clip->setStart (std::max (timeline.getTimeFromX ((event.x - localDragStart.x) + getX()), 0.0));
    else if (dragmode == dragLength)
        clip->setLength (timeline.getTimeFromX (event.x));

    if (filmstrip)
    {
        int line = (event.y + getY() - 30) / 90.0;
        if (line != clip->getVideoLine())
            clip->setVideoLine (line);
    }

    parent->resized();
}

void TimeLine::ClipComponent::mouseUp (const MouseEvent& event)
{
    dragmode = notDragging;
}
