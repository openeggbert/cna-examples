// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechniqueCollection.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::EffectReflectionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Graphics::BasicEffect;

// XNA's Effect reflection API -- Parameters, Techniques, Passes, Annotations --
// exists in CNA in full, and CNA's own built-in effects populate almost none of
// it. That is the single most useful thing this screen can say, because it is
// exactly what breaks when XNA code is ported:
//
//     effect.Parameters["WorldViewProj"]->SetValue(matrix);   // nullptr deref
//
// In XNA, BasicEffect's parameters come out of a compiled .fxb, so that lookup
// finds something. CNA's BasicEffect is hand-written C++ whose state lives in
// typed fields and property setters -- World, DiffuseColor, VertexColorEnabled
// -- and it never adds a single EffectParameter. The collection is empty, and
// the name lookup returns a null pointer rather than throwing.
//
// The collections are not decorative: Effect's own constructor adds one
// technique called "Default", and anything that builds an effect can Add()
// parameters of its own. See the EffectParameter screen for that side of it.
//
// Everything below is measured from the live objects, not asserted from memory.
class ReflectionSurfaceScreen : public DemoScreen {
public:
    ReflectionSurfaceScreen() : DemoScreen("Effect Reflection: What Is Actually Exposed") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        effect_.emplace(device);

        parameterCount_ = effect_->getParametersProperty().getCountProperty();
        techniqueCount_ = effect_->getTechniquesProperty().getCountProperty();

        if (auto* current = effect_->getCurrentTechniqueProperty()) {
            currentTechnique_ = current->getNameProperty();
            passCount_ = current->getPassesProperty().getCountProperty();
            annotationCount_ = current->getAnnotationsProperty().getCountProperty();
        }

        // The porting trap itself, exercised rather than described: a by-name
        // lookup that XNA code would immediately dereference.
        lookupIsNull_ = effect_->getParametersProperty()["World"] == nullptr;

        claimHolds_ = parameterCount_ == 0 && techniqueCount_ == 1 && lookupIsNull_;
    }

    void OnDemoUnload() override { effect_.reset(); }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        lines.push_back("Live BasicEffect on this device:");
        lines.push_back("  Parameters.Count      " + std::to_string(parameterCount_) +
                        "        Techniques.Count  " + std::to_string(techniqueCount_));
        lines.push_back("  CurrentTechnique      \"" + currentTechnique_ + "\"  passes " +
                        std::to_string(passCount_) + ", annotations " +
                        std::to_string(annotationCount_));
        lines.push_back("  Parameters[\"World\"]   " +
                        std::string(lookupIsNull_ ? "nullptr" : "found"));
        lines.emplace_back();
        lines.push_back("CNA's built-in effects never populate that collection -- their state");
        lines.push_back("lives in typed C++ fields and property setters instead:");
        lines.push_back("  effect.World = m;   effect.VertexColorEnabled = true;   (fields)");
        lines.emplace_back();
        lines.push_back("So this valid XNA line dereferences null here -- the lookup returns");
        lines.push_back("nullptr and does NOT throw, so it must be checked:");
        lines.push_back("  effect.Parameters[\"WorldViewProj\"]->SetValue(m);");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(claimHolds_ ? Color(40, 200, 90, 255) : Color(230, 170, 40, 255),
                        TransitionAlpha()),
                    tint,
                    claimHolds_ ? "Verified live: 0 parameters, 1 technique, null name lookup."
                                : "This backend reports something else -- numbers above are live.");
    }

private:
    std::optional<BasicEffect> effect_;
    std::string currentTechnique_ = "(none)";
    int parameterCount_ = 0;
    int techniqueCount_ = 0;
    int passCount_ = 0;
    int annotationCount_ = 0;
    bool lookupIsNull_ = false;
    bool claimHolds_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::EffectReflectionDemos
