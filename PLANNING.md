# TASKS.md — Reverb Plugin

Turn the Guava template into a functional stereo reverb plugin.

## Phase 1: Parameters & DSP

### 1.1 Add AudioProcessorValueTreeState (APVTS) to PluginProcessor
- Add a `juce::AudioProcessorValueTreeState` member and a static `createParameterLayout()` method.
- Define parameters:
  - **Room Size** — `roomSize`, 0.0–1.0, default 0.5
  - **Damping** — `damping`, 0.0–1.0, default 0.5
  - **Wet Level** — `wetLevel`, 0.0–1.0, default 0.33
  - **Dry Level** — `dryLevel`, 0.0–1.0, default 0.4
  - **Width** — `width`, 0.0–1.0, default 1.0
  - **Freeze** — `freeze`, boolean toggle, default off
- Pass the APVTS to the `AudioProcessor` base constructor.

### 1.2 Implement reverb DSP in processBlock
- Add a `juce::dsp::Reverb` member to PluginProcessor (already available via `juce_dsp` link).
- In `prepareToPlay()`, call `reverb.reset()` and pass a `juce::dsp::ProcessSpec`.
- In `processBlock()`:
  - Read current parameter values from APVTS into a `juce::dsp::Reverb::Parameters` struct.
  - Call `reverb.setParameters()`.
  - Wrap the buffer in a `juce::dsp::AudioBlock` and process through `reverb`.
- Update `getTailLengthSeconds()` to return a reasonable value (e.g. 2–3 seconds).

### 1.3 Implement state save/restore
- In `getStateInformation()`, serialize the APVTS to XML via `apvts.copyState()` and write to `destData`.
- In `setStateInformation()`, parse XML and call `apvts.replaceState()`.
- This enables DAW session recall and preset saving.

## Phase 2: GUI

### 2.1 Build the editor layout
- Replace the "Hello World" paint text with the plugin name as a header label.
- Add a `juce::Slider` (rotary style) + `juce::Label` for each parameter: Room Size, Damping, Wet, Dry, Width.
- Add a `juce::ToggleButton` for Freeze.
- Keep the Melatonin Inspector button for debug builds.
- Resize the editor window to fit (e.g. 500x350).

### 2.2 Attach sliders to parameters
- Use `juce::AudioProcessorValueTreeState::SliderAttachment` to bind each slider to its APVTS parameter.
- Use `juce::AudioProcessorValueTreeState::ButtonAttachment` for the Freeze toggle.
- Store attachments as `std::unique_ptr` members in PluginEditor.

### 2.3 Layout in resized()
- Arrange knobs in a row (or grid) using `juce::FlexBox` or manual `getLocalBounds()` subdivision.
- Place Freeze toggle and Inspector button below the knobs.

## Phase 3: Testing

### 3.1 Update existing tests
- Update `tests/PluginBasics.cpp` to verify the plugin still instantiates correctly with the new APVTS constructor.

### 3.2 Add parameter tests
- Test that all parameters exist in the APVTS with correct IDs, ranges, and defaults.
- Test that `getStateInformation()` / `setStateInformation()` round-trips parameter values correctly.

### 3.3 Add DSP tests
- Test that a silent input buffer remains silent when wet is 0 and dry is 1.
- Test that processBlock doesn't crash for mono and stereo layouts.
- Test that the reverb tail produces non-zero output after input stops (feed a buffer of impulse, then process empty buffers and verify output is non-zero).

### 3.4 Add editor test
- Use `runWithinPluginEditor()` from `tests/helpers/test_helpers.h` to verify the editor creates and destroys without errors.

## Phase 4: Polish

### 4.1 Tune default parameter values
- Build standalone, load audio, and adjust defaults so the reverb sounds reasonable out of the box.

### 4.2 Visual polish
- Apply consistent LookAndFeel to knobs (colours, sizing).
- Add parameter value readouts below each knob (e.g. percentage display).

### 4.3 Freeze behavior
- Verify freeze mode holds the reverb tail indefinitely and releases cleanly when toggled off.
