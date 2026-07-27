// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "CNA/LogCategory.hpp"
#include "CNA/Logger.hpp"
#include "CNA/LogLevel.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Diagnostics::LoggingDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using CNA::LogCategory;
using CNA::Logger;
using CNA::LogLevel;

// CNA::Logger is the framework's own diagnostic channel, routed to SDL's
// logging. The part worth demonstrating is the level filter, because it decides
// what a shipping build costs:
//
//   - SetMinimumLevel drops everything below it.
//   - The *If() variants pair a condition with a level in one call. Mind the
//     argument order: Log takes (level, message, category) and WarnIf takes
//     (message, condition) -- the message comes first, not the condition.
//
// Note what is NOT available: Logger::IsEnabled and Logger::ToString are both
// private in CNA (everything from Logger.hpp:201 down is), so an application
// cannot ask whether a level would survive the filter, and cannot get a level's
// name. This screen therefore compares against GetMinimumLevel itself -- which
// IS public -- and carries its own name table. That is a real constraint on the
// API, not an oversight in the demo.
//
// There are TWO filters, and that is the real lesson here. CNA keeps its own
// minimum level, and SDL keeps a separate per-category priority. CNA only
// pushes its level into SDL inside SetMinimumLevel (via SDL_SetLogPriorities),
// so until an application calls that, SDL's own defaults still govern what
// actually reaches the terminal.
//
// Measured on this build: at startup CNA's minimum is TRACE (Debug builds), so
// all six levels pass CNA -- yet only four reach stderr, because SDL is still
// filtering Debug and Trace itself. Press the key once and all six appear.
//
// The column below therefore says what passes CNA's filter, which is the only
// half an application can see; the note underneath explains the rest.
class LoggerScreen : public DemoScreen {
public:
    LoggerScreen() : DemoScreen("Diagnostics: Logger") {}

    void OnDemoLoad() override {
        originalLevel_ = Logger::GetMinimumLevel();
        Emit();
    }

    void OnDemoUnload() override {
        // The minimum level is process-global; leaving it lowered would flood
        // the console for the rest of the session.
        Logger::SetMinimumLevel(originalLevel_);
    }

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            levelIndex_ = (levelIndex_ + 1) % kLevelCount;
            Logger::SetMinimumLevel(kLevels[levelIndex_]);
            sawSetMinimumLevel_ = true;
            Emit();
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: raise or lower the minimum level");
        lines.emplace_back();
        lines.push_back("Logger::GetMinimumLevel() = " + std::string(Name(Logger::GetMinimumLevel())));
        lines.push_back("Restored to " + std::string(Name(originalLevel_)) +
                        " when you leave this screen.");
        lines.emplace_back();
        lines.push_back("Every level below was emitted for real; which passes CNA's filter:");
        for (const auto& line : emitted_) lines.push_back("  " + line);
        lines.emplace_back();
        lines.push_back("Emissions this session: " + std::to_string(emissions_));
        lines.emplace_back();
        lines.push_back("A filtered-out call still pays for its message: the string is built");
        lines.push_back("before Log() is entered. With IsEnabled private, an application cannot");
        lines.push_back("cheaply guard that -- it has to compare against GetMinimumLevel itself.");
        lines.emplace_back();
        lines.push_back("Two filters, not one. SDL keeps its own per-category priority, and CNA");
        lines.push_back("only pushes its level into SDL inside SetMinimumLevel. Before the first");
        lines.push_back("such call, Debug and Trace pass CNA and are still dropped by SDL --");
        lines.push_back(sawSetMinimumLevel_
                            ? "you have now called it, so both filters agree."
                            : "press the key once and they start appearing on the terminal.");
        lines.emplace_back();
        lines.push_back("Output goes to SDL's log: it appears on the terminal, not on this screen.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    static constexpr int kLevelCount = 4;
    static constexpr LogLevel kLevels[kLevelCount] = {
        LogLevel::INFO, LogLevel::WARN, LogLevel::ERROR, LogLevel::TRACE,
    };

    // Logger::ToString is private, so the names live here. Lower numeric value
    // means higher severity, which is what makes the filter comparison work.
    static const char* Name(LogLevel level) {
        switch (level) {
            case LogLevel::FATAL:      return "Fatal";
            case LogLevel::ERROR:      return "Error";
            case LogLevel::WARN:       return "Warn";
            case LogLevel::INFO:       return "Info";
            case LogLevel::DEBUG:      return "Debug";
            case LogLevel::TRACE:      return "Trace";
            case LogLevel::EXPERIMENT: return "Experiment";
        }
        return "?";
    }

    // Logger::IsEnabled is private too, so this reproduces the rule it applies.
    static bool WouldPass(LogLevel level) {
        return (int)level <= (int)Logger::GetMinimumLevel();
    }

    void Emit() {
        emitted_.clear();
        emissions_++;

        static const LogLevel rows[] = {
            LogLevel::FATAL, LogLevel::ERROR, LogLevel::WARN,
            LogLevel::INFO,  LogLevel::DEBUG, LogLevel::TRACE,
        };

        for (LogLevel level : rows) {
            // Emitted regardless, so the filter -- not this screen -- decides.
            // Signature is Log(level, message, category) -- message second.
            Logger::Log(level, std::string("cna-examples Logger demo: ") + Name(level),
                        LogCategory::APPLICATION);
            const std::string name = Name(level);
            emitted_.push_back(name + std::string(name.size() < 7 ? 7 - name.size() : 1, ' ') +
                               (WouldPass(level) ? "passes CNA" : "filtered out"));
        }

        // The conditional form, which pairs a test with the level.
        // And WarnIf takes the message FIRST, the condition second -- the
        // reverse of the order the name suggests.
        Logger::WarnIf("WarnIf fired on an even emission count", emissions_ % 2 == 0);
        emitted_.push_back("WarnIf(emissions % 2 == 0)   condition was " +
                           std::string(emissions_ % 2 == 0 ? "true" : "false"));
    }

    std::vector<std::string> emitted_;
    LogLevel originalLevel_ = LogLevel::INFO;
    int levelIndex_ = 0;
    int emissions_ = 0;
    bool sawSetMinimumLevel_ = false;
};

} // namespace CnaExamples::Demos::Diagnostics::LoggingDemos
