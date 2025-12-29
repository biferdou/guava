# Building Guava on macOS

This guide covers building the Guava audio plugin on macOS.

## Prerequisites

### Required

1. **Xcode** - Install from the Mac App Store or [Apple Developer](https://developer.apple.com/xcode/)
   ```bash
   # Verify installation
   xcode-select -p

   # If not installed, install command line tools
   xcode-select --install
   ```

2. **CMake 3.25+**
   ```bash
   brew install cmake
   ```

3. **Ninja** (recommended for faster builds)
   ```bash
   brew install ninja
   ```

## Getting the Source

Clone the repository with submodules:

```bash
git clone --recursive https://github.com/your-repo/guava.git
cd guava
```

If you already cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

To update submodules later:

```bash
git submodule update --remote --merge
```

## Building

### Configure

```bash
# Using Ninja (faster)
cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Release

# Or using Xcode generator
cmake -B Builds -G Xcode
```

### Build All Targets

```bash
cmake --build Builds --config Release
```

### Build Specific Formats

```bash
cmake --build Builds --target Guava_Standalone
cmake --build Builds --target Guava_VST3
cmake --build Builds --target Guava_AU
cmake --build Builds --target Guava_CLAP
```

## Build Outputs

After building, plugins are located in:

```
Builds/Guava_artefacts/Release/
├── AU/Guava.component
├── AUv3/Guava.appex
├── VST3/Guava.vst3
├── CLAP/Guava.clap
└── Standalone/Guava.app
```

By default, plugins are automatically copied to your user plugin directories:
- AU: `~/Library/Audio/Plug-Ins/Components/`
- VST3: `~/Library/Audio/Plug-Ins/VST3/`

## Running Tests

```bash
# Build and run all tests
cmake --build Builds --target Tests
ctest --test-dir Builds --output-on-failure

# Run a specific test
./Builds/Tests_artefacts/Tests "[test-name]"
```

## Running Benchmarks

```bash
cmake --build Builds --target Benchmarks
./Builds/Benchmarks_artefacts/Benchmarks
```

## Universal Binaries (arm64 + x86_64)

To build for both Apple Silicon and Intel:

```bash
cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build Builds --config Release
```

## Xcode Development

To generate an Xcode project for development:

```bash
cmake -B Builds -G Xcode
open Builds/Guava.xcodeproj
```

The project includes schemes for the Standalone app and Tests target.

## Local Development (No Code Signing)

For local development and testing, code signing is **not required**. The built plugins will work on your own machine without signing.

If macOS blocks an unsigned plugin:

1. Open **System Preferences → Security & Privacy → General**
2. Click "Allow Anyway" for the blocked plugin
3. Or run this to remove the quarantine attribute:
   ```bash
   xattr -cr "Builds/Guava_artefacts/Release/VST3/Guava.vst3"
   xattr -cr "Builds/Guava_artefacts/Release/AU/Guava.component"
   ```

## Code Signing (Distribution Only)

Code signing is only needed when distributing plugins to other users. Required certificates:
- Developer ID Application certificate
- Developer ID Installer certificate (for .pkg installers)

Sign a plugin:

```bash
codesign --force -s "Developer ID Application: Your Name (TEAM_ID)" \
  -v "Builds/Guava_artefacts/Release/VST3/Guava.vst3" \
  --deep --strict --options=runtime --timestamp
```

## Troubleshooting

### Xcode Version Issues

JUCE requires a recent Xcode version. If you encounter build errors:

```bash
# List available Xcode versions
ls /Applications/ | grep Xcode

# Switch to a specific version
sudo xcode-select -s /Applications/Xcode.app/Contents/Developer
```

### Clean Build

If you encounter strange build issues:

```bash
rm -rf Builds
cmake -B Builds -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build Builds
```

### Plugin Not Loading in DAW

1. Check Console.app for loading errors
2. Verify the plugin is signed (for macOS 10.15+)
3. Try removing and re-copying the plugin to the plugin directory