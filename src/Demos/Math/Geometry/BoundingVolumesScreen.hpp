// SPDX-License-Identifier: MIT
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/ContainmentType.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::GeometryDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::BoundingBox;
using Microsoft::Xna::Framework::BoundingSphere;
using Microsoft::Xna::Framework::ContainmentType;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// Intersects() and Contains() answer different questions, and the difference is
// the whole point of ContainmentType:
//
//   Intersects -> bool: "do these overlap at all?"
//   Contains   -> ContainmentType: Disjoint / Intersects / Contains.
//
// A broad-phase test wants the bool. Anything that needs to know whether an
// object is *fully* inside -- frustum culling that skips per-triangle work,
// trigger volumes that fire only on full entry -- needs the three-way answer.
//
// A sphere slides through a fixed box here, so all three states are reached in
// sequence and the transitions are visible rather than sampled.
class BoundingVolumesScreen : public DemoScreen {
public:
    BoundingVolumesScreen() : DemoScreen("Geometry: Bounding Volumes") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        travel_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.8f;
        if (travel_ > 8.0f) travel_ -= 8.0f;
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const BoundingBox box(Vector3(-1.5f, -1.5f, -1.5f), Vector3(1.5f, 1.5f, 1.5f));

        // Sweeps from -4 to +4 and back.
        const float x = travel_ < 4.0f ? travel_ - 4.0f : 4.0f - (travel_ - 4.0f);
        const BoundingSphere sphere(Vector3(x, 0.0f, 0.0f), 0.8f);

        const ContainmentType boxContainsSphere = box.Contains(sphere);
        const bool intersects = box.Intersects(sphere);

        // CreateFromPoints over a fixed cloud -- the usual way a bounding volume
        // is actually produced.
        const Vector3 cloud[] = {
            Vector3(-2.0f, 0.5f, 1.0f), Vector3(1.0f, -1.0f, 0.0f),
            Vector3(0.0f, 2.0f, -1.5f), Vector3(1.5f, 0.5f, 2.0f),
        };
        const std::vector<Vector3> points(std::begin(cloud), std::end(cloud));
        const BoundingBox fromPoints = BoundingBox::CreateFromPoints(points);
        const BoundingSphere sphereFromPoints = BoundingSphere::CreateFromPoints(points);
        const BoundingSphere merged = BoundingSphere::CreateMerged(sphere, sphereFromPoints);

        std::vector<std::string> lines;
        lines.push_back("A sphere slides through a fixed box, so all three states occur in turn.");
        lines.emplace_back();
        lines.push_back("Box    Min " + V3(box.Min) + "  Max " + V3(box.Max));
        lines.push_back("Sphere Centre " + V3(sphere.Center) + "  Radius " + F(sphere.Radius));
        lines.emplace_back();
        lines.push_back("box.Intersects(sphere) -> " + std::string(intersects ? "true" : "false"));
        lines.push_back("box.Contains(sphere)   -> " + Name(boxContainsSphere));
        lines.push_back("  " + Meaning(boxContainsSphere));
        lines.emplace_back();
        lines.push_back("Note both are true while the sphere straddles a face: Intersects cannot");
        lines.push_back("distinguish 'partly in' from 'entirely in'. Contains can, and that is");
        lines.push_back("what culling needs to skip work rather than merely to include an object.");
        lines.emplace_back();
        lines.push_back("Built from a 4-point cloud:");
        lines.push_back("  BoundingBox::CreateFromPoints    Min " + V3(fromPoints.Min) +
                        " Max " + V3(fromPoints.Max));
        lines.push_back("  BoundingSphere::CreateFromPoints Centre " + V3(sphereFromPoints.Center) +
                        " R " + F(sphereFromPoints.Radius));
        lines.push_back("  CreateMerged(moving, cloud)      Centre " + V3(merged.Center) +
                        " R " + F(merged.Radius));

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawDiagram(sb, box, sphere, boxContainsSphere);
    }

private:
    static std::string Name(ContainmentType type) {
        switch (type) {
            case ContainmentType::Disjoint:   return "Disjoint";
            case ContainmentType::Contains:   return "Contains";
            case ContainmentType::Intersects: return "Intersects";
        }
        return "?";
    }

    static std::string Meaning(ContainmentType type) {
        switch (type) {
            case ContainmentType::Disjoint:   return "no overlap at all";
            case ContainmentType::Contains:   return "the sphere is entirely inside the box";
            case ContainmentType::Intersects: return "they overlap, but the sphere is not fully inside";
        }
        return "";
    }

    void DrawDiagram(SpriteBatch& sb, const BoundingBox& box, const BoundingSphere& sphere,
                     ContainmentType state) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();
        const PlotSpace plot{Vector2((float)viewport.getWidthProperty() - 160.0f, 250.0f), 26.0f};

        const Vector2 topLeft = plot.ToScreen(Vector2(box.Min.X, box.Max.Y));
        const Vector2 bottomRight = plot.ToScreen(Vector2(box.Max.X, box.Min.Y));
        FillRect(sb, Rectangle((int)topLeft.X, (int)topLeft.Y,
                               (int)(bottomRight.X - topLeft.X), (int)(bottomRight.Y - topLeft.Y)),
                 mul(Color(45, 55, 70), alpha));

        const Color sphereColor = state == ContainmentType::Contains   ? Color(90, 200, 120)
                                  : state == ContainmentType::Intersects ? Color(230, 190, 70)
                                                                         : Color(180, 90, 90);
        const Vector2 centre = plot.ToScreen(Vector2(sphere.Center.X, sphere.Center.Y));
        const int r = (int)(sphere.Radius * plot.unitsToPixels);
        FillRect(sb, Rectangle((int)centre.X - r, (int)centre.Y - r, r * 2, r * 2),
                 mul(sphereColor, alpha));
    }

    float travel_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::GeometryDemos
