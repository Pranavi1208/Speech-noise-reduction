# Speech Noise Reduction in C

An audio noise-filtering program written in C. It reads a WAV file, reduces
background noise using three different DSP techniques, and writes the filtered
audio to a new WAV file.

Author: Pranavi Nallaparaju

## Filters implemented

| Filter | Idea |
| ------ | ---- |
| **Moving average** | Replaces each sample with the average of its neighbours. Simple, but it smooths the speech as well as the noise. |
| **Wiener filter** | Estimates the noise power from non-speech parts of the recording, then scales each sample by a gain `signal / (signal + noise)`. |
| **Spectral subtraction** | Splits the audio into 256-sample frames, converts each frame to the frequency domain with an FFT, subtracts the estimated noise spectrum, keeps the original phase, and converts back with an inverse FFT. |

The Wiener filter and spectral subtraction use a simple **speech detector**:
the audio is split into windows, the average amplitude of each window is
compared to a threshold (40% of the loudest window), and windows below it are
treated as background noise. Those windows are used to estimate the noise.
Frames detected as speech are passed through unchanged.

The full write-up with flow diagrams is in [`docs/documentation.pdf`](docs/documentation.pdf).

## Project structure

```
include/   header files (noise_filter.h, KISS FFT headers)
src/       source code (main.c, one file per filter, WAV reader, speech detection)
input/     input recording (audio2.wav)
output/    filtered results
docs/      project documentation
```

## Build and run

```bash
gcc src/main.c \
    src/wav_reader.c \
    src/moving_average.c \
    src/wiener_filter.c \
    src/speech_detection.c \
    src/spectral_subtraction.c \
    src/kiss_fft.c \
    src/kiss_fftr.c \
    -Iinclude -lm -o noise_filter

./noise_filter
```

Run it from the project root so the `input/` and `output/` paths resolve.
Spectral subtraction is enabled by default and writes
`output/spectral_subtraction2-frame256.wav`.

### Switching filters

`src/main.c` contains all three filters. To try another one, comment out the
enabled section, uncomment the one you want, recompile and run. Enable only
one at a time.

## Results

Measured on the 2-second, 16 kHz test recording (`input/audio2.wav`). The
number shows how much the remaining background noise dropped in the frames the
speech detector labelled as non-speech (RMS level, in dB).

| Filter | Noise reduction | Effect on speech |
| ------ | --------------- | ---------------- |
| Spectral subtraction | about 2.2 dB | unchanged |
| Wiener filter | about 1.9 dB | unchanged |
| Moving average | about 1.3 dB | speech level drops about 12% (muffled) |

These are modest improvements on one short recording, not a benchmark. Moving
average reduced noise the least and also dulled the speech, which is why the
other two filters use speech detection.

## Limitations

- Expects 16-bit mono PCM WAV files with a standard 44-byte header.
- Frames do not overlap and no window function is applied, so some artefacts are possible.
- The speech detector is a simple amplitude threshold and can misclassify very quiet speech.
- Speech frames are copied unchanged, so noise that occurs during speech is not removed.
- Assumes the number of samples is a multiple of the frame size (true for the included clip).

## Third-party code

FFT routines come from [KISS FFT](https://github.com/mborgerding/kissfft) by
Mark Borgerding (BSD-3-Clause licence). The copyright notice is kept in each
KISS FFT source file. Everything else was written by me.
