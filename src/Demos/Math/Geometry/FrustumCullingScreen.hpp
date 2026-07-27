// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/BoundingFrustum.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/ContainmentType.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Plane.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::GeometryDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::BoundingFrustum;
using Microsoft::Xna::Framework::BoundingSphere;
using Microsoft::Xna::Framework::ContainmentType;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Plane;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// Frustum culling is the single highest-value use of the math API: skip
// everything the camera cannot see.
//
// A BoundingFrustum is constructed from a view*projection matrix -- not from
// six planes -- and it extracts the planes itself. That is worth showing,
// because it means the frustum automatically follows the camera with no
// bookkeeping.
//
// The grid below is culled against a rotating camera every frame and the
// visible/culled counts are real. Contains() is used rather than Intersects()
// so partly-visible objects are distinguishable from fully-visible ones, which
// is what lets a real renderer skip per-object clipping work.
class FrustumCullingScreen : public DemoScreen {
public:
    FrustumCullingScreen() : DemoScreen("Geometry: Frustum Culling") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.4f;
        angle_ = MathHelper::WrapAngle(angle_);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector3 cameraPosition(0.0f, 0.0f, 0.0f);
        const Vector3 lookAt(std::cos(angle_) * 10.0f, 0.0f, std::sin(angle_) * 10.0f);
        const Matrix view = Matrix::CreateLookAt(cameraPosition, lookAt, Vector3::Up);
        const Matrix projection = Matrix::CreatePerspectiveFieldOfView(
            MathHelper::ToRadians(60.0f), 1.4f, 0.5f, 14.0f);

        const BoundingFrustum frustum(view * projection);

        int inside = 0;
        int partial = 0;
        int outside = 0;
        std::vector<std::pair<Vector3, ContainmentType>> results;

        for (int gz = -5; gz <= 5; ++gz) {
            for (int gx = -5; gx <= 5; ++gx) {
                const Vector3 centre((float)gx * 2.0f, 0.0f, (float)gz * 2.0f);
                const BoundingSphere sphere(centre, 0.6f);
                const ContainmentType state = frustum.Contains(sphere);
                switch (state) {
                    case ContainmentType::Contains:   inside++;  break;
                    case ContainmentType::Intersects: partial++; break;
                    case ContainmentType::Disjoint:   outside++; break;
                }
                results.emplace_back(centre, state);
            }
        }

        const int total = inside + partial + outside;
        const auto corners = frustum.GetCorners();

        std::vector<std::string> lines;
        lines.push_back("An 11x11 grid of spheres, culled against a rotating camera every frame.");
        lines.emplace_back();
        lines.push_back("Camera looks at " + V3(lookAt) + "   FOV 60, near 0.5, far 14");
        lines.emplace_back();
        lines.push_back("Contains(sphere) over " + std::to_string(total) + " objects:");
        lines.push_back("  fully inside  " + std::to_string(inside));
        lines.push_back("  intersecting  " + std::to_string(partial));
        lines.push_back("  culled        " + std::to_string(outside) + "   (" +
                        F(100.0f * (float)outside / (float)total, 0) + "% of the scene skipped)");
        lines.emplace_back();
        lines.push_back("The frustum is built from view * projection and extracts its own planes:");
        lines.push_back("  Near   normal " + V3(frustum.getNearProperty().Normal) +
                        "  D " + F(frustum.getNearProperty().D));
        lines.push_back("  Far    normal " + V3(frustum.getFarProperty().Normal) +
                        "  D " + F(frustum.getFarProperty().D));
        lines.push_back("  GetCorners() returned " + std::to_string((int)corners.size()) +
                        " points (4 near + 4 far)");
        lines.emplace_back();
        lines.push_back("Contains, not Intersects: a renderer that knows an object is FULLY");
        lines.push_back("inside can skip per-object clipping entirely, not merely draw it.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawTopDown(sb, results, cameraPosition, lookAt);
    }

private:
    void DrawTopDown(SpriteBatch& sb,
                     const std::vector<std::pair<Vector3, ContainmentType>>& results,
                     const Vector3& camera, const Vector3& lookAt) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();
        const PlotSpace plot{Vector2((float)viewport.getWidthProperty() - 145.0f, 260.0f), 10.5f};

        for (const auto& [centre, state] : results) {
            const Color color = state == ContainmentType::Contains     ? Color(90, 200, 120)
                                : state == ContainmentType::Intersects ? Color(230, 190, 70)
                                                                       : Color(70, 70, 80);
            // X across, Z down: a top-down plan view of the scene.
            const Vector2 point = plot.ToScreen(Vector2(centre.X, -centre.Z));
            FillRect(sb, Rectangle((int)point.X - 3, (int)point.Y - 3, 7, 7), mul(color, alpha));
        }

        const Vector2 eye = plot.ToScreen(Vector2(camera.X, -camera.Z));
        const Vector2 target = plot.ToScreen(Vector2(lookAt.X, -lookAt.Z));
        DrawArrow(sb, eye, target, mul(Color(200, 120, 200), alpha), 2.0f);
    }

    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::GeometryDemos
