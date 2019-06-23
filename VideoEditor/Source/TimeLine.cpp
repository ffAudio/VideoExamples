/*
  ==============================================================================

    Copyright (c) 2019, Foleys Finest Audio - Daniel Walz
    All rights reserved.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.

  ==============================================================================

    TimeLine.cpp
    Created: 30 Mar 2019 4:46:00pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"

#include "Player.h"
#include "Properties.h"
#include "TimeLine.h"

namespace IDs
{
    static Identifier videoLine { "videoLine" };
    static Identifier audioLine { "audioLine" };
}

//==============================================================================
TimeLine::TimeLine (foleys::VideoEngine& theVideoEngine, Player& playerToUse, Properties& properiesToUse)
  : videoEngine (theVideoEngine),
    player (playerToUse),
    properties (properiesToUse)
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
    for (int i=0; i < numVideoLines; ++i)
        g.fillRect (0, 10 + i * 90, getWidth(), 80);

    g.setColour (Colours::darkgrey.darker());
    for (int i=0; i < numAudioLines; ++i)
        g.fillRect (0, 200 + i * 60, getWidth(), 50);
}

void TimeLine::resized()
{
    if (edit == nullptr)
        return;

    auto sampleRate = getSampleRate();
    if (sampleRate == 0)
        sampleRate = 48000.0;

    timelineLength = std::max (60.0, edit->getLengthInSeconds() * 1.1);

    for (auto& component : clipComponents)
    {
        if (component->isVideoClip())
        {
            int videoline = component->clip->getStatusTree().getProperty (IDs::videoLine, 0);
            component->setBounds (getXFromTime (component->clip->getStart()), 10 + videoline * 90,
                                  getXFromTime (component->clip->getLength()), 80);
        }
        else
        {
            int audioline = component->clip->getStatusTree().getProperty (IDs::audioLine, 0);
            component->setBounds (getXFromTime (component->clip->getStart()), 200 + audioline * 60,
                                  getXFromTime (component->clip->getLength()), 50);
        }
    }

    auto tx = getXFromTime (player.getCurrentTimeInSeconds());
    timemarker.setBounds (tx, 0, 3, getHeight());
}

void TimeLine::timecodeChanged (int64_t count, double seconds)
{
    ignoreUnused (time);
    auto tx = getXFromTime (seconds);
    timemarker.setBounds (tx, 0, 3, getHeight());
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

    addClipToEdit (files [0], getTimeFromX (x), y);
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
        addClipToEdit (source->getSelectedFile(), getTimeFromX (dragSourceDetails.localPosition.x), dragSourceDetails.localPosition.y);
    }
}

void TimeLine::addClipToEdit (juce::File file, double start, int y)
{
    auto length = -1.0;
    auto clip = videoEngine.createClipFromFile (file);

    if (clip.get() == nullptr)
    {
        AlertWindow::showNativeDialogBox (NEEDS_TRANS ("Loading failed"), NEEDS_TRANS (""), true);
        return;
    }

    if (std::dynamic_pointer_cast<foleys::ImageClip>(clip) != nullptr)
        length = 3.0;

    auto descriptor = edit->addClip (clip, start, length);
    if (y < 190)
    {
        int line = (y - 10) / 90.0;
        setVideoLine (descriptor, line);
    }
    else
    {
        int line = (y - 200) / 60.0;
        setAudioLine (descriptor, line);
    }

    restoreClipComponents();

    setSelectedClip (descriptor, descriptor->clip->hasVideo());
    resized();
}

void TimeLine::setSelectedClip (std::shared_ptr<foleys::ClipDescriptor> clip, bool video)
{
    selectedClip = clip;
    selectedIsVideo = video;
    properties.showClipProperties (videoEngine, clip, video);
    repaint();
}

std::shared_ptr<foleys::ClipDescriptor> TimeLine::getSelectedClip() const
{
    return selectedClip.lock();
}

bool TimeLine::selectedClipIsVideo() const
{
    return selectedIsVideo;
}

void TimeLine::toggleVisibility()
{
    auto clip = selectedClip.lock();
    if (clip.get() == nullptr)
        return;

    if (selectedIsVideo)
        clip->setVideoVisible (! clip->getVideoVisible());
    else
        clip->setAudioPlaying (! clip->getAudioPlaying());

    repaint();
}

void TimeLine::spliceSelectedClipAtPlayPosition()
{
    spliceSelectedClipAtPosition (player.getCurrentTimeInSeconds());
}

void TimeLine::spliceSelectedClipAtPosition (double pts)
{
    auto clip = selectedClip.lock();
    if (clip.get() == nullptr)
        return;

    auto start  = clip->getStart();
    auto length = clip->getLength();
    auto offset = clip->getOffset() + (pts - start);

    if (pts < start || pts > start + length)
        return;

    edit->getStatusTree().addChild (clip->getStatusTree().createCopy(), -1, videoEngine.getUndoManager());
    clip->setLength (pts - clip->getStart());
    clip->updateSampleCounts();

    auto newClip = edit->getClip (int (edit->getClips().size()) - 1);
    if (newClip.get() == nullptr)
        return;

    newClip->setStart (pts);
    newClip->setLength (length - (pts - start));
    newClip->setOffset (offset);

    newClip->setDescription (edit->makeUniqueDescription(clip->getDescription()));
    newClip->updateSampleCounts();
}

void TimeLine::restoreClipComponents()
{
    if (edit == nullptr)
        return;

    auto clips = edit->getClips();
    for (auto descriptor : clips)
    {
        if (descriptor->clip->hasVideo())
        {
            auto comp = std::find_if (clipComponents.begin(), clipComponents.end(), [descriptor](const auto& comp){ return comp->isVideoClip() && comp->clip == descriptor; });
            if (comp == clipComponents.end())
                addClipComponent (descriptor, true);
        }

        if (descriptor->clip->hasAudio())
        {
            auto comp = std::find_if (clipComponents.begin(), clipComponents.end(), [descriptor](const auto& comp){ return !comp->isVideoClip() && comp->clip == descriptor; });
            if (comp == clipComponents.end())
                addClipComponent (descriptor, false);
        }
    }

    clipComponents.erase (std::remove_if (clipComponents.begin(),
                                          clipComponents.end(),
                                          [&](auto& component)
                                          {
                                              auto findClipWithComponent = [&](const auto& clip){ return clip == component->clip; };
                                              return std::find_if (clips.begin(), clips.end(), findClipWithComponent) == clips.end();
                                          }), clipComponents.end());

    resized();
}

void TimeLine::addClipComponent (std::shared_ptr<foleys::ClipDescriptor> clip, bool video)
{
    auto strip = std::make_unique<ClipComponent> (*this, clip, videoEngine.getThreadPool(), video);
    addAndMakeVisible (strip.get());
    clipComponents.emplace_back (std::move (strip));
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
    return (seconds / timelineLength) * getWidth();
}

double TimeLine::getTimeFromX (int pixels) const
{
    auto w = getWidth();
    return w > 0 ? timelineLength * pixels / w : 0;
}

int TimeLine::getVideoLine (const std::shared_ptr<foleys::ClipDescriptor> descriptor) const
{
    return descriptor->getStatusTree().getProperty (IDs::videoLine, 0);
}

int TimeLine::getAudioLine (const std::shared_ptr<foleys::ClipDescriptor> descriptor) const
{
    return descriptor->getStatusTree().getProperty (IDs::audioLine, 0);
}

void TimeLine::setVideoLine (std::shared_ptr<foleys::ClipDescriptor> descriptor, int lane) const
{
    descriptor->getStatusTree().setProperty (IDs::videoLine,
                                             jlimit (0, numVideoLines - 1, lane),
                                             videoEngine.getUndoManager());
}

void TimeLine::setAudioLine (std::shared_ptr<foleys::ClipDescriptor> descriptor, int lane) const
{
    descriptor->getStatusTree().setProperty (IDs::audioLine,
                                             jlimit (0, numAudioLines - 1, lane),
                                             videoEngine.getUndoManager());
}

double TimeLine::getSampleRate() const
{
    return player.getSampleRate();
}

void TimeLine::valueTreePropertyChanged (juce::ValueTree& treeWhosePropertyHasChanged,
                                         const juce::Identifier& property)
{
    MessageManager::callAsync ([safeComponent = SafePointer<TimeLine> (this)] () mutable { if (safeComponent) safeComponent->resized(); });
}

void TimeLine::valueTreeChildAdded (juce::ValueTree& parentTree,
                                    juce::ValueTree& childWhichHasBeenAdded)
{
    MessageManager::callAsync ([safeComponent = SafePointer<TimeLine> (this)] () mutable { if (safeComponent) safeComponent->restoreClipComponents(); });
}

void TimeLine::valueTreeChildRemoved (juce::ValueTree& parentTree,
                                      juce::ValueTree& childWhichHasBeenRemoved,
                                      int indexFromWhichChildWasRemoved)
{
    MessageManager::callAsync ([safeComponent = SafePointer<TimeLine> (this)] () mutable { if (safeComponent) safeComponent->restoreClipComponents(); });
}

//==============================================================================

TimeLine::ClipComponent::ClipComponent (TimeLine& tl,
                                        std::shared_ptr<foleys::ClipDescriptor> clipToUse,
                                        ThreadPool& threadPool, bool video)
  : clip (clipToUse),
    timeline (tl)
{
    if (video)
    {
        filmstrip = std::make_unique<foleys::FilmStrip>();
        filmstrip->setClip (clip->clip);
        addAndMakeVisible (filmstrip.get());
    }
    else
    {
        audiostrip = std::make_unique<foleys::AudioStrip>();
        audiostrip->setClip (clip->clip);
        addAndMakeVisible (audiostrip.get());
    }
}

void TimeLine::ClipComponent::paint (Graphics& g)
{
    bool selected = timeline.getSelectedClip() == clip;

    g.fillAll (Colours::darkgrey);

    auto colour = isVideoClip() ? Colours::orange : Colours::darkgreen;

    g.setColour (selected ? colour : colour.darker());
    g.fillRoundedRectangle (getLocalBounds().reduced (1).toFloat(), 5.0);

    if (selected)
        g.setColour (timeline.selectedIsVideo == isVideoClip() ? colour.contrasting() : colour.contrasting().withAlpha (0.5f));
    else
        g.setColour (colour);

    g.drawRoundedRectangle (getLocalBounds().toFloat(), 5.0, 2.0);
    if (clip == nullptr)
        return;

    g.drawFittedText (clip->getDescription(), 5, 3, getWidth() - 10, 18, Justification::left, 1);

    if (filmstrip.get() != nullptr)
        filmstrip->setAlpha (clip->getVideoVisible() ? 1.0f : 0.5f);

    if (audiostrip.get() != nullptr)
        audiostrip->setAlpha (clip->getAudioPlaying() ? 1.0f : 0.3f);
}

void TimeLine::ClipComponent::resized()
{
    if (clip == nullptr)
        return;

    if (filmstrip)
    {
        filmstrip->setBounds (1, 20, getWidth() - 2, getHeight() - 25);
        filmstrip->setStartAndLength (clip->getOffset(), clip->getLength() + clip->getOffset());
    }
    if (audiostrip)
    {
        audiostrip->setBounds (1, 20, getWidth() - 2, getHeight() - 25);
        audiostrip->setStartAndLength (clip->getOffset(), clip->getLength() + clip->getOffset());
    }
}

void TimeLine::ClipComponent::mouseMove (const MouseEvent& event)
{
    if (event.x > getWidth() - 5 || event.x < 5)
        setMouseCursor (MouseCursor::LeftRightResizeCursor);
    else
        setMouseCursor (MouseCursor::DraggingHandCursor);
}

void TimeLine::ClipComponent::mouseDown (const MouseEvent& event)
{
    localDragStart = event.getPosition();
    timeline.setSelectedClip (clip, isVideoClip());

    if (event.x < 5)
        dragmode = dragOffset;
    else if (event.x > getWidth() - 5)
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
        clip->setLength (std::min (timeline.getTimeFromX (event.x), clip->clip->getLengthInSeconds()));
    else if (dragmode == dragOffset)
    {
        auto oldPosition = clip->getStart();
        auto oldOffset   = clip->getOffset();
        auto oldLength   = clip->getLength();
        auto delta = std::min (std::max (timeline.getTimeFromX ((event.x - localDragStart.x) + getX()) - oldPosition,
                                         -oldOffset), oldLength);
        clip->setStart (oldPosition + delta);
        clip->setOffset (oldOffset + delta);
        clip->setLength (oldLength - delta);
        clip->updateSampleCounts();
    }

    if (filmstrip)
    {
        int line = (event.y + getY() - 10) / 90.0;
        if (line != timeline.getVideoLine (clip))
            timeline.setVideoLine (clip, line);
    }
    else
    {
        int line = (event.y + getY() - 200) / 60.0;
        if (line != timeline.getAudioLine (clip))
            timeline.setAudioLine (clip, line);
    }

    parent->resized();
}

void TimeLine::ClipComponent::mouseUp (const MouseEvent& event)
{
    dragmode = notDragging;
}

bool TimeLine::ClipComponent::isVideoClip() const
{
    return filmstrip.get() != nullptr;
}
