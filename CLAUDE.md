# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Guava is a JUCE audio plugin built using the Pamplejuce template. It produces Standalone, AU, VST3, AUv3, and CLAP plugin formats. The codebase uses C++23 and JUCE 8.x.

## Build Commands

```bash
# Configure (from project root)
cmake -B Builds -DCMAKE_BUILD_TYPE=Release

# Build all targets
cmake --build Builds

# Build specific target
cmake --build Builds --target Guava_Standalone
cmake --build Builds --target Guava_VST3
cmake --build Builds --target Guava_AU

# Run tests
cmake --build Builds --target Tests
ctest --test-dir Builds --output-on-failure

# Run a single test by tag
./Builds/Tests_artefacts/Tests "[instance]"

# Run benchmarks
cmake --build Builds --target Benchmarks
./Builds/Benchmarks_artefacts/Benchmarks

# Format code (JUCE-style, see .clang-format)
clang-format -i source/*.cpp source/*.h
```

## Architecture

### Core Plugin Structure
- `source/PluginProcessor.h/.cpp` - Audio processing logic (inherits `juce::AudioProcessor`). Stereo in/out, mono/stereo bus layouts supported.
- `source/PluginEditor.h/.cpp` - GUI implementation (inherits `juce::AudioProcessorEditor`). Includes Melatonin Inspector button for UI debugging.
- `createPluginFilter()` in PluginProcessor.cpp is the plugin entry point (creates `PluginProcessor` instances).

### Build System
- CMake-based with CPM for dependency management
- JUCE 8.x and Pamplejuce cmake helpers are git submodules — run `git submodule update --init --recursive` after cloning
- Plugin version controlled via `/VERSION` file (propagates to JUCE and installers)
- Source files are auto-discovered via `GLOB_RECURSE` on `source/` directory
- Assets in `assets/` are automatically bundled as `BinaryData` (accessible via `#include "BinaryData.h"`)

### Target Structure
- `SharedCode` - INTERFACE library containing all plugin source, linked by both plugin and test targets (avoids ODR violations)
- `Guava` - Main plugin target (produces all format binaries)
- `Tests` - Test executable using Catch2 v3 (tags: `[dummy]`, `[instance]`)
- `Benchmarks` - Separate benchmark executable using Catch2

### Key Dependencies
- **JUCE** - Audio plugin framework (submodule in `/JUCE`)
- **Catch2 v3** - Testing framework (fetched via CPM)
- **Melatonin Inspector** - UI debugging tool (in `/modules/melatonin_inspector`)
- **clap-juce-extensions** - CLAP format support (in `/modules/clap-juce-extensions`)

### Testing
- Tests use Catch2 v3 with `TEST_CASE` / `SECTION` / `CHECK` / `REQUIRE` macros
- `tests/helpers/test_helpers.h` provides `runWithinPluginEditor()` for tests requiring a plugin editor context (handles editor lifecycle)
- Test target has `JUCE_MODAL_LOOPS_PERMITTED=1` to allow Message Manager usage
- Test source auto-discovered from `tests/` directory

### Compile Definitions Available in Source
- `PRODUCT_NAME_WITHOUT_VERSION` - Plugin display name ("Guava")
- `VERSION` - Current version string from `/VERSION`
- `CMAKE_BUILD_TYPE` - "Release" or "Debug"

## Code Style
- JUCE-style formatting enforced via `.clang-format` (Allman braces, 4-space indent, no column limit)
- Space before non-empty parentheses: `void foo (int x)` not `void foo(int x)`
