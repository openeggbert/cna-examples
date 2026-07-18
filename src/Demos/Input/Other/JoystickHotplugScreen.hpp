// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "CNA/Input/Joysticks.hpp"
#include "System/MulticastAction.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::Joysticks;
using JoystickHotplugToken = System::MulticastAction<std::uint32_t>::Token;

// Demonstrates the NOXNA/EXT Joysticks::ConnectedEXT/DisconnectedEXT
// hot-plug events: plug in or unplug a raw joystick/wheel/flight stick
// while this screen is open to see it logged live. Same subscribe-in-
// LoadContent/unsubscribe-in-UnloadContent lifetime discipline as every
// other MulticastAction subscription in this app.
class JoystickHotplugScreen : public DemoScreen {
public:
    JoystickHotplugScreen() : DemoScreen("Other: Joystick Hot-plug Events (EXT)") {}

    void LoadContent() override {
        connectedToken_ = Joysticks::ConnectedEXT.Add([this](std::uint32_t id) {
            Log("Connected: id " + std::to_string(id));
        });
        disconnectedToken_ = Joysticks::DisconnectedEXT.Add([this](std::uint32_t id) {
            Log("Disconnected: id " + std::to_string(id));
        });
    }

    void UnloadContent() override {
        Joysticks::ConnectedEXT.Remove(connectedToken_);
        Joysticks::DisconnectedEXT.Remove(disconnectedToken_);
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Plug in or unplug a raw joystick/wheel/flight stick; events log below.");
        lines.emplace_back();
        for (const auto& line : log_) lines.push_back(line);

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    void Log(std::string line) {
        log_.push_back(std::move(line));
        while (log_.size() > kMaxLines) log_.pop_front();
    }

    static constexpr std::size_t kMaxLines = 14;

    JoystickHotplugToken connectedToken_ = System::MulticastAction<std::uint32_t>::InvalidToken;
    JoystickHotplugToken disconnectedToken_ = System::MulticastAction<std::uint32_t>::InvalidToken;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::OtherDemos
