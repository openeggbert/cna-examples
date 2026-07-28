// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadCapabilities.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Input::GamepadDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Input::GamePad;

// The three EXT entry points that talk to a modern pad's non-button hardware:
// its motion sensors and its light bar. All three are FNA extensions, not XNA
// -- the 360 pad these APIs were originally shaped around had none of it.
//
// Their failure modes differ, and that is the point worth showing:
//
//   GetGyroEXT / GetAccelerometerEXT  return BOOL. False means "this pad has
//       no such sensor", which is the normal case, not an error. They fill the
//       out-parameter only when they return true, so reading the vector
//       without checking the return value gives you stale data.
//
//   SetLightBarEXT                    returns VOID. There is no way to ask
//       whether it did anything. On a pad without a light bar it is simply a
//       no-op, and on a disconnected one it is also a no-op. Write-only
//       hardware with no feedback channel -- do not build logic on it.
//
// Space cycles the light bar colour so the call is genuinely exercised rather
// than merely described; on a DualShock/DualSense the bar visibly changes.
class MotionAndLightBarScreen : public DemoScreen {
public:
    MotionAndLightBarScreen() : DemoScreen("Gamepad: Motion Sensors & Light Bar (EXT)") {}

protected:
    void OnDemoInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(ControllingPlayer(), playerIndex)) {
            colorIndex_ = (colorIndex_ + 1) % kColorCount;
            GamePad::SetLightBarEXT(PlayerIndex::One, kColors[colorIndex_]);
            ++lightBarCalls_;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());

        Vector3 gyro(0.0f, 0.0f, 0.0f);
        Vector3 accel(0.0f, 0.0f, 0.0f);
        const bool hasGyro = GamePad::GetGyroEXT(PlayerIndex::One, gyro);
        const bool hasAccel = GamePad::GetAccelerometerEXT(PlayerIndex::One, accel);
        const bool connected = GamePad::GetState(PlayerIndex::One).getIsConnectedProperty();

        std::vector<std::string> lines;
        lines.push_back("PlayerIndex::One is " +
                        std::string(connected ? "connected." : "NOT connected."));
        lines.emplace_back();
        lines.push_back("GetGyroEXT           returned " + YesNo(hasGyro) + "   " + Show(hasGyro, gyro));
        lines.push_back("GetAccelerometerEXT  returned " + YesNo(hasAccel) + "   " + Show(hasAccel, accel));
        lines.emplace_back();
        lines.push_back("Both return BOOL. False means this pad has no such sensor -- the normal");
        lines.push_back("case, not an error. They fill the out-parameter ONLY when true, so");
        lines.push_back("reading the vector without checking gives you stale data.");
        lines.emplace_back();
        lines.push_back("Space/Enter/A: SetLightBarEXT(" + std::string(kColorNames[colorIndex_]) +
                        ")   calls made: " + std::to_string(lightBarCalls_));
        lines.push_back("SetLightBarEXT returns VOID -- there is no way to ask whether it did");
        lines.push_back("anything. No light bar, or no controller, is a silent no-op. It is");
        lines.push_back("write-only hardware with no feedback channel; do not branch on it.");
        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);
    }

private:
    static constexpr int kColorCount = 4;
    static constexpr const char* kColorNames[kColorCount] = {"red", "green", "blue", "white"};

    static std::string YesNo(bool value) { return value ? "true " : "false"; }

    static std::string Show(bool valid, const Vector3& v) {
        if (!valid) return "(not filled in)";
        char buffer[80];
        std::snprintf(buffer, sizeof(buffer), "x %+.3f  y %+.3f  z %+.3f", v.X, v.Y, v.Z);
        return buffer;
    }

    inline static const Color kColors[kColorCount] = {
        Color(255, 40, 40, 255), Color(40, 220, 90, 255),
        Color(60, 110, 255, 255), Color(255, 255, 255, 255),
    };

    int colorIndex_ = 0;
    int lightBarCalls_ = 0;
};

} // namespace CnaExamples::Demos::Input::GamepadDemos
