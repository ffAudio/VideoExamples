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

void Properties::showClipProperties (std::shared_ptr<foleys::ClipDescriptor> clip, bool video)
{
    if (video)
        showProperties (std::make_unique<ClipVideoProperties>(clip));
    else
        showProperties (std::make_unique<ClipAudioProperties>(clip));
}

//==============================================================================

ClipAudioProperties::ClipAudioProperties (std::shared_ptr<foleys::ClipDescriptor> clipToUse) : clip (clipToUse)
{
    if (! clip->audioProcessors.empty())
    {
        editor = std::make_unique<GenericAudioProcessorEditor>(clip->audioProcessors.front()->processor.get());
        addAndMakeVisible (editor.get());
    }
}

void ClipAudioProperties::paint (Graphics& g)
{
    g.setColour (Colours::silver);
    g.drawFittedText (NEEDS_TRANS ("Audio: ") + clip->getDescription(), getLocalBounds().removeFromTop (36), Justification::left, 1);
}

void ClipAudioProperties::resized()
{
    if (editor)
        editor->setBounds (getLocalBounds().withTop (40).reduced (5));
}

//==============================================================================

ClipVideoProperties::ClipVideoProperties (std::shared_ptr<foleys::ClipDescriptor> clipToUse) : clip (clipToUse)
{
}

void ClipVideoProperties::paint (Graphics& g)
{
    g.setColour (Colours::silver);
    g.drawFittedText (NEEDS_TRANS ("Video: ") + clip->getDescription(), getLocalBounds().removeFromTop (36), Justification::left, 1);
}

