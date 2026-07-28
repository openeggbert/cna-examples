// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimationPreset.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarAnimationPresetNamesEXT.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBodyType.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AvatarBodyTypeNamesEXT.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Avatars::DescriptionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::GamerServices::AvatarAnimationPreset;
using Microsoft::Xna::Framework::GamerServices::AvatarAnimationPresetToClipNameEXT;
using Microsoft::Xna::Framework::GamerServices::AvatarBodyType;
using Microsoft::Xna::Framework::GamerServices::AvatarBodyTypeToContentNameEXT;

// Phase E: AvatarAnimationPreset has exactly 31 enumerators (8 Stand + Clap/Wave/Celebrate +
// 10 Female* + 10 Male*), each mapped 1:1 by name to a real SkinnedModelEXT::Clips key via
// AvatarAnimationPresetToClipNameEXT -- confirmed by reading the .cpp switch directly, not
// assumed. AvatarBodyTypeToContentNameEXT similarly maps the 2 AvatarBodyType values to the
// ContentManager asset names the real-rendering screens load. No 3D and no borrowed asset
// needed here -- these are pure string-mapping functions, verified without a GraphicsDevice.
class AvatarNameTablesScreen : public DemoScreen {
public:
    AvatarNameTablesScreen() : DemoScreen("Avatars: Preset & BodyType Name Tables") {}

    void OnDemoLoad() override {
        for (int i = 0; i <= (int)AvatarAnimationPreset::MaleYawn; ++i) {
            presetNames_.push_back(AvatarAnimationPresetToClipNameEXT((AvatarAnimationPreset)i));
        }
        femaleContentName_ = AvatarBodyTypeToContentNameEXT(AvatarBodyType::Female);
        maleContentName_ = AvatarBodyTypeToContentNameEXT(AvatarBodyType::Male);

        // Every preset name is unique and non-empty -- the real, checkable claim behind
        // "31 distinct presets", rather than just trusting the enumerator count.
        allNonEmpty_ = true;
        allUnique_ = true;
        for (std::size_t i = 0; i < presetNames_.size(); ++i) {
            if (presetNames_[i].empty()) allNonEmpty_ = false;
            for (std::size_t j = i + 1; j < presetNames_.size(); ++j) {
                if (presetNames_[i] == presetNames_[j]) allUnique_ = false;
            }
        }
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("AvatarAnimationPreset: " + std::to_string(presetNames_.size()) + " enumerators (expected 31),");
        lines.push_back("each mapped 1:1 by AvatarAnimationPresetToClipNameEXT() to its own SkinnedModelEXT clip name.");
        lines.push_back("First 3: " + presetNames_[0] + ", " + presetNames_[1] + ", " + presetNames_[2]);
        lines.push_back("Last 3:  " + presetNames_[presetNames_.size() - 3] + ", " +
                        presetNames_[presetNames_.size() - 2] + ", " + presetNames_[presetNames_.size() - 1]);
        lines.emplace_back();
        lines.push_back("AvatarBodyTypeToContentNameEXT(Female) = \"" + femaleContentName_ + "\"");
        lines.push_back("AvatarBodyTypeToContentNameEXT(Male)   = \"" + maleContentName_ + "\"");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));

        const bool pass = presetNames_.size() == 31 && allNonEmpty_ && allUnique_ &&
                          femaleContentName_ == "avatar/female/avatar" && maleContentName_ == "avatar/male/avatar";
        DrawVerdict(sb, font, end.Y + 10.0f, pass ? Color(60, 200, 90, 255) : Color(210, 60, 60, 255),
                   mul(Color::White, TransitionAlpha()),
                   pass ? "PASS: 31 unique preset names; both asset names correct"
                        : "FAIL: name table did not match expectations");
    }

private:
    std::vector<std::string> presetNames_;
    std::string femaleContentName_;
    std::string maleContentName_;
    bool allNonEmpty_ = false;
    bool allUnique_ = false;
};

} // namespace CnaExamples::Demos::Avatars::DescriptionDemos
