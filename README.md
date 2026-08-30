# SkyVerb — Room Reverb (VST3)

A from-scratch JUCE plugin recreating the first algorithm of a
multi-algorithm hall/room reverb in the style of a Strymon Big Sky —
branded as its own product, "SkyVerb", to keep clear of Strymon's
trademarks. GUI styled after a turquoise brushed-aluminium pedal look,
with a two-column LED algorithm selector flanking the encoder.

This was built and **successfully compiled to a working Linux VST3 and
Standalone app** in the dev sandbox (see `BigSkyClone-linux-build.tar.gz`
if you just want to hear it on Linux). To use it in a real DAW on
**Mac or Windows**, you need to build it there — plugin binaries aren't
cross-platform.

## What's implemented

- **DSP** (`Source/RoomReverb.h`): pre-delay → 4-stage input diffusion
  (series allpass) → 8-line Feedback Delay Network with a Householder
  feedback matrix, per-line damping, and subtle per-line LFO modulation
  to avoid metallic ringing. Stereo output is derived from alternating
  FDN lines.
- **Parameters** (`Source/PluginProcessor.cpp`): Decay, Pre-Delay, Tone,
  Mix, Param 1 (Size), Param 2 (Diffusion), Mod — all exposed as
  automatable VST3 parameters via `AudioProcessorValueTreeState`.
- **GUI** (`Source/PluginEditor.cpp`, `Source/BigSkyLookAndFeel.h`):
  custom-painted rotary knobs with a thin brass accent ring, turquoise
  gradient background with brushed-metal streaks and a vignette,
  monospace green LCD readout, a two-column algorithm selector (with
  LEDs) flanking the encoder, "SkyVerb / Multi-Dimensional Reverb"
  branding, and footswitch graphics with BANK DOWN / BANK UP labels —
  laid out to resemble the reference mockup you shared.
- Only the **Room** algorithm exists so far. The left-hand "VALUE"
  encoder and the two-column algorithm list (Bloom, Swell, Spring,
  Plate, Hall, Chorale, Shimmer, Magneto, Nonlinear, Reflections) are
  drawn as a preview — only Room's LED lights up, since that's the
  only one implemented. Adding a real algorithm is the hook for later:
  another class alongside `RoomReverb`, plus wiring the encoder to
  actually switch between them.
- Footswitches A/B/C recall or save preset snapshots: click to recall a
  slot, Shift+click to save the current knob state into it. A green LED
  above each footswitch lights up once that slot has something saved.

## Building via GitHub Actions (no local install needed)

This project includes a ready-made workflow at
`.github/workflows/build.yml` that builds the Windows and macOS VST3
for you on GitHub's own servers — no Visual Studio or Xcode install
required on your end.

1. Create a free GitHub account if you don't have one, and create a
   new empty repository (public or private both work).
2. Push this project folder to it:
   ```bash
   cd SkyVerbClone          # the extracted project folder
   git init
   git add .
   git commit -m "Initial commit"
   git branch -M main
   git remote add origin https://github.com/<your-username>/<your-repo>.git
   git push -u origin main
   ```
3. On GitHub, open your repo's **Actions** tab. The "Build VST3"
   workflow starts automatically after the push (or click
   **Run workflow** to trigger it manually).
4. Wait for it to finish (a few minutes — it's compiling the whole
   JUCE framework from source). Click into the finished run, and
   under **Artifacts** you'll find `SkyVerb-Windows-VST3` (and
   `SkyVerb-macOS-VST3`) as downloadable zips.
5. Unzip and copy the `.vst3` folder inside into your system's VST3
   folder (`C:\Program Files\Common Files\VST3\` on Windows), then
   rescan plugins in your DAW.

## Building on macOS or Windows locally

You need: CMake 3.22+, a C++ compiler (Xcode on Mac / Visual Studio 2022
on Windows), and Git.

```bash
git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git
# put the JUCE folder next to CMakeLists.txt (or edit the path in CMakeLists.txt)

cmake -B build -G "Xcode"              # macOS, or:
cmake -B build -G "Visual Studio 17 2022"   # Windows

cmake --build build --config Release --target BigSkyClone_VST3
```

The resulting `.vst3` will be under
`build/BigSkyClone_artefacts/Release/VST3/`. Copy it to your system's
VST3 folder:
- macOS: `~/Library/Audio/Plug-Ins/VST3/`
- Windows: `C:\Program Files\Common Files\VST3\`

Or build `BigSkyClone_Standalone` to get a runnable app with no DAW
needed.

## Building on Linux (already verified working here)

```bash
sudo apt-get install -y libasound2-dev libgtk-3-dev libcurl4-openssl-dev \
    libfontconfig1-dev libx11-dev libfreetype-dev
cmake -B build -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --target BigSkyClone_VST3 -j
```

## Known rough edges / next steps

- `VALUE`/`TYPE` and the two-column algorithm list are visual
  placeholders for future algorithm switching — only Room is real.
- Reverb tuning (delay line lengths, damping curve, RT60 mapping) is a
  reasonable starting point but will benefit from ear-tuning against a
  real Big Sky or reference recordings.
- Not yet tested inside a DAW on real audio — only compiled and linked
  successfully. Load it and listen before trusting the sound.
