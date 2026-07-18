// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "CNA/Input/InputDevices.hpp"
#include "System/MulticastAction.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::OtherDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Input::InputDevices;
using DeviceHotplugToken = System::MulticastAction<std::uint32_t>::Token;

// Demonstrates the NOXNA/EXT mouse/keyboard hot-plug events on
// CNA::Input::InputDevices: plug in or unplug a mouse or keyboard while
// this screen is open to see it logged live. Four separate events (Mouse
// Connected/Disconnected, Keyboard Connected/Disconnected), all
// subscribed in LoadContent() and unsubscribed in UnloadContent().
class InputDeviceHotplugScreen : public DemoScreen {
public:
    InputDeviceHotplugScreen() : DemoScreen("Other: Device Hot-plug Events (EXT)") {}

    void LoadContent() override {
        mouseConnected_ = InputDevices::MouseConnectedEXT.Add(
            [this](std::uint32_t id) { Log("Mouse connected: id " + std::to_string(id)); });
        mouseDisconnected_ = InputDevices::MouseDisconnectedEXT.Add(
            [this](std::uint32_t id) { Log("Mouse disconnected: id " + std::to_string(id)); });
        keyboardConnected_ = InputDevices::KeyboardConnectedEXT.Add(
            [this](std::uint32_t id) { Log("Keyboard connected: id " + std::to_string(id)); });
        keyboardDisconnected_ = InputDevices::KeyboardDisconnectedEXT.Add(
            [this](std::uint32_t id) { Log("Keyboard disconnected: id " + std::to_string(id)); });
    }

    void UnloadContent() override {
        InputDevices::MouseConnectedEXT.Remove(mouseConnected_);
        InputDevices::MouseDisconnectedEXT.Remove(mouseDisconnected_);
        InputDevices::KeyboardConnectedEXT.Remove(keyboardConnected_);
        InputDevices::KeyboardDisconnectedEXT.Remove(keyboardDisconnected_);
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Plug in or unplug a mouse or keyboard; events are logged below.");
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

    DeviceHotplugToken mouseConnected_ = System::MulticastAction<std::uint32_t>::InvalidToken;
    DeviceHotplugToken mouseDisconnected_ = System::MulticastAction<std::uint32_t>::InvalidToken;
    DeviceHotplugToken keyboardConnected_ = System::MulticastAction<std::uint32_t>::InvalidToken;
    DeviceHotplugToken keyboardDisconnected_ = System::MulticastAction<std::uint32_t>::InvalidToken;
    std::deque<std::string> log_;
};

} // namespace CnaExamples::Demos::Input::OtherDemos
