// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <vector>

#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace CnaExamples::Demos::Audio {

using SharpRuntime::bytecs;
using SharpRuntime::intcs;

// Generates a mono, 16-bit PCM sine wave -- the raw material every demo in
// this Area plays, since Audio has no Content-loadable asset shipped with
// this app (unlike the baked menu SpriteFont): SoundEffect's raw-PCM-buffer
// constructor makes a real, audible tone fully self-contained, with no WAV
// file dependency. A short fade-in/out is applied to avoid a click at the
// buffer edges.
inline std::vector<bytecs> GenerateSineWavePcm16(float frequencyHz, float durationSeconds,
                                                  intcs sampleRate = 44100) {
    const intcs sampleCount = (intcs)(durationSeconds * (float)sampleRate);
    std::vector<bytecs> buffer((std::size_t)sampleCount * 2);

    const intcs fadeSamples = sampleCount / 20 > 0 ? sampleCount / 20 : 1;

    for (intcs i = 0; i < sampleCount; ++i) {
        float t = (float)i / (float)sampleRate;
        float amplitude = 0.5f;
        if (i < fadeSamples) amplitude *= (float)i / (float)fadeSamples;
        if (i > sampleCount - fadeSamples) amplitude *= (float)(sampleCount - i) / (float)fadeSamples;

        const float sample = amplitude * std::sin(2.0f * 3.14159265f * frequencyHz * t);
        const auto pcm = (std::int16_t)(sample * 32767.0f);

        buffer[(std::size_t)i * 2 + 0] = (bytecs)(pcm & 0xFF);
        buffer[(std::size_t)i * 2 + 1] = (bytecs)((pcm >> 8) & 0xFF);
    }

    return buffer;
}

} // namespace CnaExamples::Demos::Audio
