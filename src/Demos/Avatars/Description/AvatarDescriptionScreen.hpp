// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/AvatarBodyType.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarDescription.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Avatars::DescriptionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::AvatarBodyType;
using Microsoft::Xna::Framework::GamerServices::AvatarDescription;

// Phase E: AvatarDescription's own header comment (verified below, not just quoted) states the
// real XNA behavior this class preserves exactly rather than "fixing" -- despite the name,
// CreateRandom() never actually randomizes anything. Both overloads always return an all-zero,
// 1021-byte, IsValid==false description, and the bodyType argument to the 1-arg overload is never
// read. Height/BodyType lazily default to 0/Female on first access rather than reading the
// (always-zero) description bytes. No 3D and no borrowed asset needed -- pure C++ API surface.
class AvatarDescriptionScreen : public DemoScreen {
public:
    AvatarDescriptionScreen() : DemoScreen("AvatarDescription: CreateRandom & IsValid") {}

    void OnDemoLoad() override {
        const AvatarDescription plain = AvatarDescription::CreateRandom();
        const AvatarDescription gendered = AvatarDescription::CreateRandom(AvatarBodyType::Male);

        plainValid_ = plain.getIsValidProperty();
        genderedValid_ = gendered.getIsValidProperty();

        const std::vector<SharpRuntime::bytecs> bytes = plain.getDescriptionProperty();
        byteCount_ = (int)bytes.size();
        allZero_ = true;
        for (auto b : bytes) {
            if (b != 0) { allZero_ = false; break; }
        }

        plainHeight_ = plain.getHeightProperty();
        plainBodyType_ = plain.getBodyTypeProperty();
        // The 1-arg overload's bodyType argument is documented as never read -- confirm that
        // directly: a description "created" as Male still lazily defaults to Female on first
        // access, the same as the 0-arg overload, since neither ever reads real data.
        genderedBodyType_ = gendered.getBodyTypeProperty();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("AvatarDescription::CreateRandom() despite its name never randomizes anything --");
        lines.push_back("verified live below, not assumed from the doc comment.");
        lines.emplace_back();
        lines.push_back("CreateRandom():          IsValid = " + std::string(plainValid_ ? "true" : "false") +
                        " (expected false)");
        lines.push_back("CreateRandom(Male):      IsValid = " + std::string(genderedValid_ ? "true" : "false") +
                        " (expected false)");
        lines.push_back("Description bytes: " + std::to_string(byteCount_) + " (expected 1021), all-zero = " +
                        std::string(allZero_ ? "true" : "false"));
        lines.push_back("Height (lazy default): " + std::to_string(plainHeight_) + " (expected 0)");
        lines.push_back("BodyType (lazy default), CreateRandom():     " +
                        std::string(plainBodyType_ == AvatarBodyType::Female ? "Female" : "Male") +
                        " (expected Female)");
        lines.push_back("BodyType (lazy default), CreateRandom(Male): " +
                        std::string(genderedBodyType_ == AvatarBodyType::Female ? "Female" : "Male") +
                        " (expected Female -- the Male argument is never read)");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        const bool pass = !plainValid_ && !genderedValid_ && byteCount_ == 1021 && allZero_ &&
                          plainHeight_ == 0.0f && plainBodyType_ == AvatarBodyType::Female &&
                          genderedBodyType_ == AvatarBodyType::Female;
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: CreateRandom() confirmed to never randomize"
                        : "FAIL: behavior did not match the XNA-faithful contract");
    }

private:
    bool plainValid_ = true;
    bool genderedValid_ = true;
    int byteCount_ = 0;
    bool allZero_ = false;
    float plainHeight_ = -1.0f;
    AvatarBodyType plainBodyType_ = AvatarBodyType::Male;
    AvatarBodyType genderedBodyType_ = AvatarBodyType::Male;
};

} // namespace CnaExamples::Demos::Avatars::DescriptionDemos
