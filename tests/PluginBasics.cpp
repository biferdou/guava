#include "helpers/test_helpers.h"
#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

TEST_CASE ("Plugin instance", "[instance]")
{
    PluginProcessor testPlugin;

    SECTION ("name")
    {
        CHECK_THAT (testPlugin.getName().toStdString(),
            Catch::Matchers::Equals ("Guava"));
    }

    SECTION ("tail length is non-zero for reverb")
    {
        CHECK (testPlugin.getTailLengthSeconds() > 0.0);
    }
}

TEST_CASE ("APVTS parameters exist with correct defaults", "[parameters]")
{
    PluginProcessor plugin;

    SECTION ("roomSize")
    {
        auto* param = plugin.apvts.getRawParameterValue ("roomSize");
        REQUIRE (param != nullptr);
        CHECK_THAT (param->load(), Catch::Matchers::WithinAbs (0.5f, 0.01f));
    }

    SECTION ("damping")
    {
        auto* param = plugin.apvts.getRawParameterValue ("damping");
        REQUIRE (param != nullptr);
        CHECK_THAT (param->load(), Catch::Matchers::WithinAbs (0.5f, 0.01f));
    }

    SECTION ("wetLevel")
    {
        auto* param = plugin.apvts.getRawParameterValue ("wetLevel");
        REQUIRE (param != nullptr);
        CHECK_THAT (param->load(), Catch::Matchers::WithinAbs (0.33f, 0.01f));
    }

    SECTION ("dryLevel")
    {
        auto* param = plugin.apvts.getRawParameterValue ("dryLevel");
        REQUIRE (param != nullptr);
        CHECK_THAT (param->load(), Catch::Matchers::WithinAbs (0.4f, 0.01f));
    }

    SECTION ("width")
    {
        auto* param = plugin.apvts.getRawParameterValue ("width");
        REQUIRE (param != nullptr);
        CHECK_THAT (param->load(), Catch::Matchers::WithinAbs (1.0f, 0.01f));
    }

    SECTION ("freeze")
    {
        auto* param = plugin.apvts.getRawParameterValue ("freeze");
        REQUIRE (param != nullptr);
        CHECK (param->load() < 0.5f);
    }
}

TEST_CASE ("State save and restore round-trips parameter values", "[state]")
{
    PluginProcessor plugin;

    // Set non-default values
    plugin.apvts.getParameter ("roomSize")->setValueNotifyingHost (0.8f);
    plugin.apvts.getParameter ("damping")->setValueNotifyingHost (0.2f);

    // Save state
    juce::MemoryBlock stateData;
    plugin.getStateInformation (stateData);

    // Create a new plugin and restore state
    PluginProcessor plugin2;
    plugin2.setStateInformation (stateData.getData(), static_cast<int> (stateData.getSize()));

    CHECK_THAT (plugin2.apvts.getRawParameterValue ("roomSize")->load(),
        Catch::Matchers::WithinAbs (0.8f, 0.01f));
    CHECK_THAT (plugin2.apvts.getRawParameterValue ("damping")->load(),
        Catch::Matchers::WithinAbs (0.2f, 0.01f));
}

TEST_CASE ("processBlock does not crash", "[dsp]")
{
    PluginProcessor plugin;
    plugin.prepareToPlay (44100.0, 512);

    SECTION ("stereo")
    {
        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();
        juce::MidiBuffer midi;
        plugin.processBlock (buffer, midi);
    }

    plugin.releaseResources();
}

TEST_CASE ("Silent input stays silent when wet is 0", "[dsp]")
{
    PluginProcessor plugin;
    plugin.enableAllBuses();
    plugin.prepareToPlay (44100.0, 512);

    plugin.apvts.getParameter ("wetLevel")->setValueNotifyingHost (0.0f);
    plugin.apvts.getParameter ("dryLevel")->setValueNotifyingHost (1.0f);

    juce::AudioBuffer<float> buffer (2, 512);
    buffer.clear();
    juce::MidiBuffer midi;
    plugin.processBlock (buffer, midi);

    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            CHECK (std::abs (buffer.getSample (ch, i)) < 1e-10f);
        }
    }

    plugin.releaseResources();
}

TEST_CASE ("Reverb modifies signal when wet is enabled", "[dsp]")
{
    PluginProcessor plugin;
    plugin.enableAllBuses();
    plugin.prepareToPlay (44100.0, 512);

    plugin.apvts.getParameter ("wetLevel")->setValueNotifyingHost (0.5f);
    plugin.apvts.getParameter ("dryLevel")->setValueNotifyingHost (0.5f);
    plugin.apvts.getParameter ("roomSize")->setValueNotifyingHost (0.8f);

    // Fill buffer with a constant signal
    juce::AudioBuffer<float> buffer (2, 512);
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            buffer.setSample (ch, i, 0.5f);

    juce::MidiBuffer midi;

    // Process several blocks to let the reverb build up
    for (int block = 0; block < 5; ++block)
    {
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                buffer.setSample (ch, i, 0.5f);
        plugin.processBlock (buffer, midi);
    }

    // Now send a silent block — if reverb is active, the tail should produce output
    buffer.clear();
    plugin.processBlock (buffer, midi);

    float maxSample = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            maxSample = std::max (maxSample, std::abs (buffer.getSample (ch, i)));

    CHECK (maxSample > 0.0f);

    plugin.releaseResources();
}

TEST_CASE ("Editor creates and destroys without errors", "[editor]")
{
    runWithinPluginEditor ([] (PluginProcessor& plugin) {
        auto* editor = plugin.getActiveEditor();
        REQUIRE (editor != nullptr);
        CHECK (editor->getWidth() == 500);
        CHECK (editor->getHeight() == 350);
    });
}

#ifdef PAMPLEJUCE_IPP
    #include <ipp.h>

TEST_CASE ("IPP version", "[ipp]")
{
    CHECK_THAT (ippsGetLibVersion()->Version, Catch::Matchers::Equals ("2022.2.0 (r0x42db1a66)"));
}
#endif
