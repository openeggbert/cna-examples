// SPDX-License-Identifier: MIT
#pragma once

#include <cstdio>
#include <deque>
#include <string>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "System/TimeSpan.hpp"

namespace CnaExamples::Demos::Framework {

using Microsoft::Xna::Framework::Game;

// Everything in this Area mutates process-global Game/GameWindow/
// GraphicsDeviceManager state -- the timestep, the window title, the back-buffer
// size. A demo that changed one and walked away would silently alter every
// screen visited afterwards, so each of them restores what it touched in
// UnloadContent(). This is the same discipline the Input area uses for
// TouchPanel::EnabledGestures and Media uses for MediaPlayer.

inline std::string FormatMs(const System::TimeSpan& span) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.2f ms", span.getTotalMillisecondsProperty());
    return buf;
}

inline std::string FormatHz(const System::TimeSpan& span) {
    const double ms = span.getTotalMillisecondsProperty();
    if (ms <= 0.0) return "unbounded";
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%.1f Hz", 1000.0 / ms);
    return buf;
}

// A short rolling history of per-frame deltas. Frame timing is the whole point
// of the Game Loop category, and a single instantaneous number tells you almost
// nothing about whether the loop is steady -- the spread is the signal.
class FrameDeltaHistory {
public:
    explicit FrameDeltaHistory(std::size_t capacity = 60) : capacity_(capacity) {}

    void Add(double milliseconds) {
        samples_.push_back(milliseconds);
        if (samples_.size() > capacity_) samples_.pop_front();
    }

    [[nodiscard]] bool Empty() const { return samples_.empty(); }
    [[nodiscard]] std::size_t Count() const { return samples_.size(); }

    [[nodiscard]] double Mean() const {
        if (samples_.empty()) return 0.0;
        double total = 0.0;
        for (double s : samples_) total += s;
        return total / (double)samples_.size();
    }

    [[nodiscard]] double Min() const {
        if (samples_.empty()) return 0.0;
        double lowest = samples_.front();
        for (double s : samples_) if (s < lowest) lowest = s;
        return lowest;
    }

    [[nodiscard]] double Max() const {
        if (samples_.empty()) return 0.0;
        double highest = samples_.front();
        for (double s : samples_) if (s > highest) highest = s;
        return highest;
    }

    void Clear() { samples_.clear(); }

    [[nodiscard]] const std::deque<double>& Samples() const { return samples_; }

private:
    std::size_t capacity_;
    std::deque<double> samples_;
};

} // namespace CnaExamples::Demos::Framework
