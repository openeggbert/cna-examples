// SPDX-License-Identifier: MIT
#pragma once

#include <optional>
#include <string>
#include <vector>

#include "CNA/Devices/Camera.hpp"
#include "CNA/Devices/CameraDeviceInfo.hpp"
#include "CNA/Devices/CameraPosition.hpp"
#include "CNA/Devices/CameraState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "Demos/DemoScreen.hpp"

namespace CnaExamples::Demos::Devices::CameraDemos {

using namespace CnaExamples::GameStateManagement;
using CNA::Devices::Camera;
using CNA::Devices::CameraDeviceInfo;
using CNA::Devices::CameraPosition;
using CNA::Devices::CameraState;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Demonstrates CNA::Devices::Camera -- no XNA/WP7 equivalent, a CNA
// extension that captures live video frames into a Texture2D. Opens the
// first available camera on entry (if any); once its permission-check
// settles into CameraState::Ready and its frame size is known, allocates a
// matching Texture2D once and polls TryAcquireFrame() every frame,
// rendering the live feed. Works on a desktop webcam as well as a mobile
// camera, unlike most of this Area -- but still worth trying on Android if
// no webcam is available here.
class CameraCaptureScreen : public DemoScreen {
public:
    CameraCaptureScreen() : DemoScreen("Camera: Live Capture") {}

    void LoadContent() override {
        supported_ = Camera::getIsSupportedProperty();
        available_ = Camera::getAvailableCamerasProperty();
        if (supported_ && !available_.empty()) {
            camera_.emplace();
        }
    }

    void UnloadContent() override {
        camera_.reset();
        frameTexture_.reset();
    }

protected:
    void OnDemoUpdate(GameTime&) override {
        if (!camera_.has_value()) return;
        if (camera_->getStateProperty() != CameraState::Ready) return;

        const int w = camera_->getFrameWidthProperty();
        const int h = camera_->getFrameHeightProperty();
        if (w <= 0 || h <= 0) return;

        if (!frameTexture_.has_value() || frameTexture_->getWidthProperty() != w ||
            frameTexture_->getHeightProperty() != h) {
            frameTexture_.emplace(GetScreenManager()->getGraphicsDeviceProperty(), w, h);
        }

        if (camera_->TryAcquireFrame(*frameTexture_)) {
            framesAcquired_++;
        }
    }

    void OnDemoDraw(const GameTime&, SpriteBatch& sb, SpriteFont& font) override {
        std::vector<std::string> lines;
        lines.push_back("Camera::IsSupported: " + std::string(supported_ ? "true" : "false"));
        lines.push_back("Available cameras: " + std::to_string(available_.size()));
        for (const auto& info : available_) {
            lines.push_back("  " + (info.Name.empty() ? "(unnamed)" : info.Name) + " -- " +
                            PositionName(info.Position));
        }
        lines.emplace_back();

        if (!camera_.has_value()) {
            lines.push_back("No camera opened (none available, or unsupported).");
        } else {
            lines.push_back(std::string("State: ") + StateName(camera_->getStateProperty()));
            lines.push_back("Frame size: " + std::to_string(camera_->getFrameWidthProperty()) + "x" +
                            std::to_string(camera_->getFrameHeightProperty()));
            lines.push_back("Frames acquired: " + std::to_string(framesAcquired_));
        }

        const Vector2 end = DrawLines(sb, font, Vector2(40.0f, 90.0f), lines, mul(Color::White, TransitionAlpha()));

        if (frameTexture_.has_value() && framesAcquired_ > 0) {
            sb.Draw(*frameTexture_, Vector2(40.0f, end.Y + 10.0f), mul(Color::White, TransitionAlpha()));
        }
    }

private:
    static const char* StateName(CameraState state) {
        switch (state) {
            case CameraState::NotSupported: return "NotSupported";
            case CameraState::Closed: return "Closed";
            case CameraState::Opening: return "Opening (permission pending)";
            case CameraState::Denied: return "Denied";
            case CameraState::Ready: return "Ready";
            default: return "Lost";
        }
    }

    static const char* PositionName(CameraPosition position) {
        switch (position) {
            case CameraPosition::FrontFacing: return "FrontFacing";
            case CameraPosition::BackFacing: return "BackFacing";
            default: return "Unknown";
        }
    }

    bool supported_ = false;
    std::vector<CameraDeviceInfo> available_;
    std::optional<Camera> camera_;
    std::optional<Texture2D> frameTexture_;
    int framesAcquired_ = 0;
};

} // namespace CnaExamples::Demos::Devices::CameraDemos
