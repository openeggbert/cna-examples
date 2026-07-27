// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameWindow.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/IGraphicsDeviceManager.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Framework::WindowDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::DisplayOrientation;
using Microsoft::Xna::Framework::GameWindow;

// DisplayOrientation is a phone-shaped API that still has a defined meaning on
// desktop. GraphicsDeviceManager::SupportedOrientations is the set an app
// declares it can handle; GameWindow::CurrentOrientation is what it is now.
//
// On desktop CNA derives the current orientation from the window's own aspect
// ratio -- wider than tall is Landscape, taller than wide is Portrait -- so
// resizing the window past square really does change it. That is the honest
// desktop behaviour, not a stub: there is no accelerometer involved.
//
// OrientationChanged is subscribed to here for the same reason, and on desktop
// it fires when that derived value flips.
class OrientationScreen : public DemoScreen {
public:
    OrientationScreen() : DemoScreen("Window: Display Orientation") {}

    void LoadContent() override {
        GameWindow& window = Window();
        lastOrientation_ = window.getCurrentOrientationProperty();
        token_ = window.OrientationChanged.Add(
            [this](System::Object*, const System::EventArgs&) { OnOrientationChanged(); });
    }

    void UnloadContent() override {
        Window().OrientationChanged.Remove(token_);
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        GameWindow& window = Window();
        const auto bounds = window.getClientBoundsProperty();

        std::vector<std::string> lines;
        lines.push_back("Resize the window past square to flip the derived orientation.");
        lines.emplace_back();
        lines.push_back("GameWindow::CurrentOrientation: " + Name(window.getCurrentOrientationProperty()));
        lines.push_back("Window is " + std::to_string(bounds.Width) + " x " +
                        std::to_string(bounds.Height) + "  -> " +
                        (bounds.Width >= bounds.Height ? "wider than tall" : "taller than wide"));
        lines.emplace_back();
        lines.push_back("GraphicsDeviceManager::SupportedOrientations:");
        lines.push_back("  " + FlagsName(SupportedOrientations()) +
                        "   (reached via Game::Services, where the manager registers itself)");
        lines.emplace_back();
        lines.push_back("OrientationChanged raised: " + std::to_string(eventCount_) + " time(s)");
        if (log_.empty()) {
            lines.push_back("  (no change yet)");
        } else {
            for (const auto& entry : log_) lines.push_back("  " + entry);
        }
        lines.emplace_back();
        lines.push_back("On desktop this is derived from the window's aspect ratio -- there is no");
        lines.push_back("accelerometer in the loop. On a phone it reflects the real device pose.");
        lines.push_back("(The Devices area's Sensors category is where the accelerometer lives.)");
        lines.emplace_back();
        lines.push_back("CNA defines four values: Default, LandscapeLeft, LandscapeRight, Portrait.");
        lines.push_back("There is no PortraitDown -- that was Windows Phone 7, not XNA 4.0 desktop.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    GameWindow& Window() const { return GetScreenManager()->getGameProperty().getWindowProperty(); }

    // The GraphicsDeviceManager registers itself in Game::Services as both
    // IGraphicsDeviceManager and IGraphicsDeviceService, so this is the
    // XNA-idiomatic way to reach it without the app handing us a pointer.
    DisplayOrientation SupportedOrientations() const {
        auto* manager = dynamic_cast<Microsoft::Xna::Framework::GraphicsDeviceManager*>(
            GetScreenManager()->getGameProperty().getServicesProperty()
                .GetService<Microsoft::Xna::Framework::IGraphicsDeviceManager>());
        return manager != nullptr ? manager->getSupportedOrientationsProperty()
                                  : DisplayOrientation::Default;
    }

    static std::string Name(DisplayOrientation orientation) {
        switch (orientation) {
            case DisplayOrientation::Default:            return "Default";
            case DisplayOrientation::LandscapeLeft:      return "LandscapeLeft";
            case DisplayOrientation::LandscapeRight:     return "LandscapeRight";
            case DisplayOrientation::Portrait:           return "Portrait";
            default: break;
        }
        return "(combination: " + std::to_string((int)orientation) + ")";
    }

    // SupportedOrientations is a flags enum, so a single name is usually wrong.
    // CNA defines exactly four values -- Default, LandscapeLeft, LandscapeRight
    // and Portrait. There is no PortraitDown: that was a Windows Phone 7 value,
    // not part of the XNA 4.0 desktop enum CNA mirrors.
    static std::string FlagsName(DisplayOrientation orientation) {
        const int bits = (int)orientation;
        if (bits == 0) return "Default (the framework picks)";

        std::string text;
        auto add = [&text](const char* name) {
            if (!text.empty()) text += " | ";
            text += name;
        };
        if (bits & (int)DisplayOrientation::LandscapeLeft)  add("LandscapeLeft");
        if (bits & (int)DisplayOrientation::LandscapeRight) add("LandscapeRight");
        if (bits & (int)DisplayOrientation::Portrait)       add("Portrait");
        return text.empty() ? ("(unknown bits: " + std::to_string(bits) + ")") : text;
    }

    void OnOrientationChanged() {
        const auto now = Window().getCurrentOrientationProperty();
        eventCount_++;
        log_.push_back("#" + std::to_string(eventCount_) + "  " + Name(lastOrientation_) +
                       "  ->  " + Name(now));
        if (log_.size() > 5) log_.erase(log_.begin());
        lastOrientation_ = now;
    }

    DisplayOrientation lastOrientation_ = DisplayOrientation::Default;
    std::vector<std::string> log_;
    int eventCount_ = 0;
    System::EventHandler<System::EventArgs>::Token token_{};
};

} // namespace CnaExamples::Demos::Framework::WindowDemos
