// SPDX-License-Identifier: MIT
#pragma once

#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Graphics3D::EffectReflectionDemos {

using namespace CnaExamples::GameStateManagement;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::BasicEffect;
using Microsoft::Xna::Framework::Graphics::Effect;

// Effect::Clone() exists so a game can share one effect's setup across objects
// that need slightly different state -- the same material at three different
// tints, without rebuilding the effect three times.
//
// Two things about CNA's version are worth knowing before relying on it.
//
// First, it returns a RAW OWNING POINTER, not a smart pointer or a value. That
// is a deliberate documented deviation: FNA returns a GC-managed Effect, and
// C++ has no equivalent, so the caller owns the result and must delete it.
// Forgetting is a leak the compiler will not mention. This screen wraps the
// result in a unique_ptr immediately, which is what calling code should do.
//
// Second, what a clone shares with its source depends on WHICH effect it is,
// and the difference is documented in CNA rather than incidental:
//   * BasicEffect and the other stock effects copy their C++ state and share
//     GPU state implicitly -- CNA caches stock-effect pipelines globally by
//     state, not per instance, so there is nothing per-instance to share.
//   * ShaderEffect is the exception. It uniquely owns a compiled program, so
//     its Clone() RECOMPILES from the same GLSL source instead of sharing.
//     Genuine sharing would need a reference-counted backend, which that NOXNA
//     extension deliberately does not have.
//
// The independence of the copied state is measured below, not assumed.
class EffectCloneScreen : public DemoScreen {
public:
    EffectCloneScreen() : DemoScreen("Effect::Clone: What Is and Is Not Copied") {}

    void OnDemoLoad() override {
        auto& device = GetScreenManager()->getGraphicsDeviceProperty();
        original_.emplace(device);
        original_->setDiffuseColorProperty(Vector3(0.9f, 0.2f, 0.2f));
        original_->setAlphaProperty(1.0f);

        // Clone() hands back an owning raw pointer. Adopt it at once.
        clone_.reset(original_->Clone());
        clonedIsDistinctObject_ = clone_.get() != static_cast<Effect*>(&*original_);

        auto* clonedBasic = dynamic_cast<BasicEffect*>(clone_.get());
        if (clonedBasic == nullptr) return;
        clonedType_ = clone_->GetTypeName();

        // State carried across at clone time...
        const Vector3 copied = clonedBasic->getDiffuseColorProperty();
        stateWasCopied_ = std::fabs(copied.X - 0.9f) < 1e-6f &&
                          std::fabs(copied.Y - 0.2f) < 1e-6f &&
                          std::fabs(copied.Z - 0.2f) < 1e-6f;

        // ...and independent afterwards. Change only the clone.
        clonedBasic->setDiffuseColorProperty(Vector3(0.2f, 0.4f, 0.9f));
        clonedBasic->setAlphaProperty(0.5f);

        const Vector3 originalNow = original_->getDiffuseColorProperty();
        originalUnchanged_ = std::fabs(originalNow.X - 0.9f) < 1e-6f &&
                             std::fabs(originalNow.Y - 0.2f) < 1e-6f &&
                             std::fabs(originalNow.Z - 0.2f) < 1e-6f &&
                             std::fabs(original_->getAlphaProperty() - 1.0f) < 1e-6f;

        const Vector3 cloneNow = clonedBasic->getDiffuseColorProperty();
        cloneChanged_ = std::fabs(cloneNow.Z - 0.9f) < 1e-6f;

        // A clone gets its own technique collection, not a pointer into the
        // source's -- otherwise switching techniques on one would move both.
        techniquesAreSeparate_ =
            &clone_->getTechniquesProperty() != &original_->getTechniquesProperty();

        allHold_ = clonedIsDistinctObject_ && stateWasCopied_ && originalUnchanged_ &&
                   cloneChanged_ && techniquesAreSeparate_;
    }

    void OnDemoUnload() override {
        clone_.reset();
        original_.reset();
    }

protected:
    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Color tint = mul(Color::White, TransitionAlpha());
        std::vector<std::string> lines;

        lines.push_back("Effect* Clone() returns a RAW OWNING pointer -- FNA returns a GC-managed");
        lines.push_back("Effect and C++ has no equivalent, so forgetting to delete it leaks:");
        lines.push_back("  std::unique_ptr<Effect> clone(original.Clone());");
        lines.emplace_back();
        lines.push_back("Measured on a live BasicEffect clone (" + clonedType_ + "):");
        lines.push_back("  distinct object " + YesNo(clonedIsDistinctObject_) +
                        "     own Techniques collection " + YesNo(techniquesAreSeparate_));
        lines.push_back("  DiffuseColor copied at clone time " + YesNo(stateWasCopied_));
        lines.push_back("  changing the clone changed it " + YesNo(cloneChanged_) +
                        ", original untouched " + YesNo(originalUnchanged_));
        lines.emplace_back();
        lines.push_back("Stock effects copy C++ state and share GPU state implicitly -- CNA caches");
        lines.push_back("stock pipelines globally by state, not per instance. ShaderEffect is the");
        lines.push_back("exception: it owns a compiled program, so Clone() RECOMPILES the GLSL.");
        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, tint);

        DrawVerdict(sb, font, end.Y + 6.0f,
                    mul(allHold_ ? Color(40, 200, 90, 255) : Color(220, 60, 60, 255),
                        TransitionAlpha()),
                    tint,
                    allHold_ ? "Verified live: clone state is copied, then independent."
                             : "One of the checks above did not hold on this backend.");
    }

private:
    static std::string YesNo(bool value) { return value ? "yes" : "no"; }

    std::optional<BasicEffect> original_;
    std::unique_ptr<Effect> clone_;
    std::string clonedType_ = "(none)";
    bool clonedIsDistinctObject_ = false;
    bool stateWasCopied_ = false;
    bool originalUnchanged_ = false;
    bool cloneChanged_ = false;
    bool techniquesAreSeparate_ = false;
    bool allHold_ = false;
};

} // namespace CnaExamples::Demos::Graphics3D::EffectReflectionDemos
