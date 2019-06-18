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

    Properties.cpp
    Created: 30 Mar 2019 4:45:22pm
    Author:  Daniel Walz

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Properties.h"
#include "ProcessorComponent.h"

//==============================================================================

Properties::Properties()
{
    addAndMakeVisible (close);

    close.onClick = [&]
    {
        closeProperties();
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

void Properties::showClipProperties (foleys::VideoEngine& engine, std::shared_ptr<foleys::ClipDescriptor> clip, bool video)
{
        showProperties (std::make_unique<ClipProcessorProperties>(engine, clip, video));
}

void Properties::closeProperties()
{
    showProperties ({});
    repaint();
}

//==============================================================================

ClipProcessorProperties::ClipProcessorProperties (foleys::VideoEngine& engineToUse,
                                                  std::shared_ptr<foleys::ClipDescriptor> clipToUse,
                                                  bool showVideo)
  : engine (engineToUse),
    clip (clipToUse),
    video (showVideo)
{
    addAndMakeVisible (processorSelect);
    if (video)
    {
        processorSelect.onClick = [&]
        {
            auto lockedClip = clip.lock();
            if (lockedClip == nullptr)
                return;

            PopupMenu menu;
            auto& manager = engine.getVideoPluginManager();
            manager.populatePluginSelection (menu);

            auto description = manager.getPluginDescriptionFromMenuID (menu.show());
            String error;
            auto processor = manager.createVideoPluginInstance (description, error);
            if (processor != nullptr)
                lockedClip->addVideoProcessor (std::move (processor));

            updateEditors();
        };
    }
    else
    {
        processorSelect.onClick = [&]
        {
            auto lockedClip = clip.lock();
            if (lockedClip == nullptr)
                return;

            PopupMenu menu;
            auto& manager = engine.getAudioPluginManager();
            manager.populatePluginSelection (menu);

            auto description = manager.getPluginDescriptionFromMenuID (menu.show());
            String error;
            auto& owningClip = lockedClip->getOwningClip();
            auto processor = manager.createAudioPluginInstance (description.createIdentifierString(), owningClip.getSampleRate(), owningClip.getDefaultBufferSize(), error);
            if (processor != nullptr)
                lockedClip->addAudioProcessor (std::move (processor));

            updateEditors();
        };
    }

    scroller.setScrollBarsShown (true, false);
    scroller.setViewedComponent (&container, false);
    addAndMakeVisible (scroller);

    updateEditors();
    auto lockedClip = clip.lock();
    if (lockedClip == nullptr)
        return;

    lockedClip->addListener (this);
}

void ClipProcessorProperties::updateEditors()
{
    auto lockedClip = clip.lock();
    if (lockedClip == nullptr)
        return;

    editors.clear();

    const auto& processors = video ? lockedClip->getVideoProcessors() : lockedClip->getAudioProcessors();

    for (auto& processor : processors)
    {
        auto editor = std::make_unique<ProcessorComponent>(*processor);
        container.addAndMakeVisible (editor.get());
        editor->timecodeChanged (0 , lockedClip->getOwningClip().getCurrentTimeInSeconds());
        editor->addChangeListener (this);
        editors.push_back (std::move (editor));
    }

    resized();
}

ClipProcessorProperties::~ClipProcessorProperties()
{
    auto lockedClip = clip.lock();
    if (lockedClip != nullptr)
        lockedClip->removeListener (this);
}

void ClipProcessorProperties::paint (Graphics& g)
{
    g.setColour (Colours::silver);
    auto lockedClip = clip.lock();
    if (lockedClip != nullptr)
        g.drawFittedText ((video ? NEEDS_TRANS ("Video: ") : NEEDS_TRANS ("Audio: ")) + lockedClip->getDescription(),
                          getLocalBounds().removeFromTop (36).reduced (5), Justification::left, 1);
}

void ClipProcessorProperties::resized()
{
    processorSelect.setBounds (getWidth() - 105, 8, 100, 26);

    auto area = getLocalBounds().withTop (40).reduced (5);
    scroller.setBounds (area);

    if (clip.expired())
    {
        editors.clear();
        return;
    }

    int needed = 0;
    for (auto& editor : editors)
        needed += editor->getHeightForWidth (area.getWidth());

    Rectangle<int> childRect (0, 0, area.getWidth() - scroller.getScrollBarThickness(), needed);
    container.setBounds (childRect);

    for (auto& editor : editors)
        editor->setBounds (childRect.removeFromTop (editor->getHeightForWidth (area.getWidth())).reduced (5));
}

void ClipProcessorProperties::processorControllerToBeDeleted (const foleys::ProcessorController* toBeDeleted)
{
    auto editor = std::find_if (editors.begin(), editors.end(), [toBeDeleted](auto& p)
                            {
                                return p->getProcessorController() == toBeDeleted;
                            });

    if (editor != editors.end())
    {
        (*editor)->removeChangeListener (this);
        editors.erase (editor);
    }

    resized();
}

void ClipProcessorProperties::changeListenerCallback (ChangeBroadcaster*)
{
    resized();
}
