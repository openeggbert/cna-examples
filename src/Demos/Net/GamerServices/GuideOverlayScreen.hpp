// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/MessageBoxIcon.hpp"
#include "System/IAsyncResult.hpp"
#include "System/NotSupportedException.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Net::GamerServicesDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::Guide;
using Microsoft::Xna::Framework::GamerServices::MessageBoxIcon;

// Demonstrates the static Guide overlay API's split personality on this platform:
// BeginShowKeyboardInput/EndShowKeyboardInput complete synchronously and never throw, but
// always return an empty string (real keystroke capture is not implemented here);
// BeginShowMessageBox always throws NotSupportedException outright; every Show* UI launcher
// (ShowSignIn, ShowMarketplace, ...) is a real, silent no-op. This matches real desktop XNA
// 4.0 too -- Guide was always a no-op off Xbox.
class GuideOverlayScreen : public DemoScreen {
public:
    GuideOverlayScreen() : DemoScreen("GamerServices: Guide Overlay") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            System::IAsyncResult* result = Guide::BeginShowKeyboardInput(
                PlayerIndex::One, "Enter name", "Type something", "default",
                System::AsyncCallback{}, std::any{});
            lastKeyboardResult_ = "\"" + Guide::EndShowKeyboardInput(result) + "\"";
            delete result;
            keyboardCount_++;
        } else if (input.IsNewKeyPress(Keys::M, ControllingPlayer(), playerIndex)) {
            messageBoxAttempts_++;
            try {
                System::IAsyncResult* result = Guide::BeginShowMessageBox(
                    "Title", "Body", {"OK"}, 0, MessageBoxIcon::None,
                    System::AsyncCallback{}, std::any{});
                delete result;
                lastMessageBoxResult_ = "returned without throwing (unexpected)";
            } catch (const System::NotSupportedException& ex) {
                lastMessageBoxResult_ = std::string("threw NotSupportedException: ") + ex.getMessageProperty();
            }
        } else if (input.IsNewKeyPress(Keys::T, ControllingPlayer(), playerIndex)) {
            Guide::setSimulateTrialModeProperty(!Guide::getSimulateTrialModeProperty());
        } else if (input.IsNewKeyPress(Keys::S, ControllingPlayer(), playerIndex)) {
            Guide::ShowSignIn(1, false);
            showSignInCount_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Space/Enter/A/tap: BeginShowKeyboardInput+End (always \"\")");
        lines.push_back("M: BeginShowMessageBox (always throws)   T: toggle SimulateTrialMode");
        lines.push_back("S: Guide::ShowSignIn() (silent no-op)");
        lines.emplace_back();
        lines.push_back("IsTrialMode: " + std::string(Guide::getIsTrialModeProperty() ? "true" : "false") +
                         "  SimulateTrialMode: " + std::string(Guide::getSimulateTrialModeProperty() ? "true" : "false"));
        lines.push_back("IsScreenSaverEnabled: " + std::string(Guide::getIsScreenSaverEnabledProperty() ? "true" : "false"));
        lines.push_back("IsVisible: " + std::string(Guide::getIsVisibleProperty() ? "true" : "false") + " (always false)");
        lines.emplace_back();
        lines.push_back("BeginShowKeyboardInput calls: " + std::to_string(keyboardCount_));
        if (!lastKeyboardResult_.empty()) lines.push_back("  last result: " + lastKeyboardResult_);
        lines.push_back("BeginShowMessageBox attempts: " + std::to_string(messageBoxAttempts_));
        if (!lastMessageBoxResult_.empty()) lines.push_back("  " + lastMessageBoxResult_);
        lines.push_back("ShowSignIn() calls: " + std::to_string(showSignInCount_) + " (silent no-op)");

        DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));
    }

private:
    int keyboardCount_ = 0;
    std::string lastKeyboardResult_;
    int messageBoxAttempts_ = 0;
    std::string lastMessageBoxResult_;
    int showSignInCount_ = 0;
};

} // namespace CnaExamples::Demos::Net::GamerServicesDemos
