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

    Properties.h
    Created: 30 Mar 2019 4:45:22pm
    Author:  Daniel Walz

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class Properties    : public Component
{
public:
    Properties();

    void paint (Graphics&) override;
    void resized() override;

    void showProperties (std::unique_ptr<Component> component);

    void showClipProperties (std::shared_ptr<foleys::ClipDescriptor> clip, bool video);

private:

    std::unique_ptr<Component> component;
    juce::TextButton close { "X" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Properties)
};


//==============================================================================

class ClipProcessorProperties  : public Component
{
public:
    ClipProcessorProperties (std::shared_ptr<foleys::ClipDescriptor> clip, bool video);

    void paint (Graphics& g) override;

    void resized() override;

private:
    std::shared_ptr<foleys::ClipDescriptor> clip;
    std::unique_ptr<AudioProcessorEditor> editor;
    bool video = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClipProcessorProperties)
};
