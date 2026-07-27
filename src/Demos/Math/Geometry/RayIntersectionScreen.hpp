// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Plane.hpp"
#include "Microsoft/Xna/Framework/Ray.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/DemoScreen.hpp"
#include "Demos/Math/MathDemoHelpers.hpp"

namespace CnaExamples::Demos::Math::GeometryDemos {

using namespace CnaExamples::GameStateManagement;
using CnaExamples::Demos::DemoScreen;
using Microsoft::Xna::Framework::BoundingBox;
using Microsoft::Xna::Framework::BoundingSphere;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::Plane;
using Microsoft::Xna::Framework::Ray;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;

// Ray::Intersects returns an optional distance ALONG the ray, not a point and
// not a bool. Two consequences that this screen makes concrete:
//
//   - The hit point is Position + Direction * distance, and that is only correct
//     if Direction is unit length. An unnormalised direction makes every
//     returned distance wrong by its magnitude, silently.
//   - A ray is a half-line. Geometry behind Position is never hit, however
//     close it is. The sweeping ray here passes objects on both sides so the
//     "behind me" case is reached and reported, not just described.
class RayIntersectionScreen : public DemoScreen {
public:
    RayIntersectionScreen() : DemoScreen("Geometry: Ray Intersection") {}

protected:
    void OnDemoUpdate(GameTime& gameTime) override {
        angle_ += (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 0.45f;
        angle_ = MathHelper::WrapAngle(angle_);
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        const Vector3 origin(-4.0f, 0.0f, 0.0f);
        const Vector3 direction =
            Vector3::Normalize(Vector3(std::cos(angle_ * 0.5f), std::sin(angle_ * 0.5f) * 0.6f, 0.0f));
        const Ray ray(origin, direction);

        const BoundingSphere sphere(Vector3(0.0f, 0.0f, 0.0f), 1.2f);
        const BoundingBox box(Vector3(2.0f, -1.0f, -1.0f), Vector3(3.5f, 1.0f, 1.0f));
        const Plane plane(Vector3::UnitY, 2.0f);   // y = -2

        const auto sphereHit = ray.Intersects(sphere);
        const auto boxHit = ray.Intersects(box);
        const auto planeHit = ray.Intersects(plane);

        // The same ray with a deliberately unnormalised direction.
        const Ray badRay(origin, Vector3(direction.X * 3.0f, direction.Y * 3.0f, 0.0f));
        const auto badHit = badRay.Intersects(sphere);

        std::vector<std::string> lines;
        lines.push_back("A ray sweeps from " + V3(origin) + "; direction rotates.");
        lines.push_back("Direction = " + V3(direction) + "   (unit length " +
                        F(direction.Length(), 3) + ")");
        lines.emplace_back();
        lines.push_back("Intersects(sphere @ origin, r=1.2): " + Describe(sphereHit, ray));
        lines.push_back("Intersects(box  x 2.0..3.5):        " + Describe(boxHit, ray));
        lines.push_back("Intersects(plane y = -2):           " + Describe(planeHit, ray));
        lines.emplace_back();
        lines.push_back("Same ray, direction scaled x3 (NOT normalised):");
        lines.push_back("  distance to sphere = " +
                        (badHit.has_value() ? F(badHit.value(), 3) : std::string("(no hit)")) +
                        (sphereHit.has_value() && badHit.has_value()
                             ? "   vs " + F(sphereHit.value(), 3) + " normalised"
                             : ""));
        lines.push_back("  The distance is returned in units of the direction vector's length,");
        lines.push_back("  so a non-unit direction quietly rescales every result.");
        lines.emplace_back();
        lines.push_back("A Ray is a half-line: nothing behind Position is ever hit, no matter");
        lines.push_back("how close. Watch the sphere result vanish as the ray turns away.");

        DrawLines(sb, font, Vector2(40.0f, 82.0f), lines, mul(Color::White, TransitionAlpha()));
        DrawDiagram(sb, ray, sphere, box, sphereHit, boxHit);
    }

private:
    static std::string Describe(const std::optional<float>& hit, const Ray& ray) {
        if (!hit.has_value()) return "no hit";
        const Vector3 point(ray.Position.X + ray.Direction.X * hit.value(),
                            ray.Position.Y + ray.Direction.Y * hit.value(),
                            ray.Position.Z + ray.Direction.Z * hit.value());
        return "distance " + F(hit.value(), 3) + "  at " + V3(point);
    }

    void DrawDiagram(SpriteBatch& sb, const Ray& ray, const BoundingSphere& sphere,
                     const BoundingBox& box, const std::optional<float>& sphereHit,
                     const std::optional<float>& boxHit) {
        auto& viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const float alpha = TransitionAlpha();
        const PlotSpace plot{Vector2(150.0f, (float)viewport.getHeightProperty() - 150.0f), 24.0f};

        const Vector2 topLeft = plot.ToScreen(Vector2(box.Min.X, box.Max.Y));
        const Vector2 bottomRight = plot.ToScreen(Vector2(box.Max.X, box.Min.Y));
        FillRect(sb, Rectangle((int)topLeft.X, (int)topLeft.Y,
                               (int)(bottomRight.X - topLeft.X), (int)(bottomRight.Y - topLeft.Y)),
                 mul(boxHit.has_value() ? Color(90, 200, 120) : Color(60, 70, 85), alpha));

        const Vector2 centre = plot.ToScreen(Vector2(sphere.Center.X, sphere.Center.Y));
        const int r = (int)(sphere.Radius * plot.unitsToPixels);
        FillRect(sb, Rectangle((int)centre.X - r, (int)centre.Y - r, r * 2, r * 2),
                 mul(sphereHit.has_value() ? Color(90, 200, 120) : Color(60, 70, 85), alpha));

        const Vector2 from = plot.ToScreen(Vector2(ray.Position.X, ray.Position.Y));
        const Vector2 to = plot.ToScreen(Vector2(ray.Position.X + ray.Direction.X * 9.0f,
                                                 ray.Position.Y + ray.Direction.Y * 9.0f));
        DrawArrow(sb, from, to, mul(Color(230, 190, 70), alpha), 2.0f);
    }

    float angle_ = 0.0f;
};

} // namespace CnaExamples::Demos::Math::GeometryDemos
