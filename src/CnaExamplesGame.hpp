// SPDX-License-Identifier: MIT
#pragma once

#include <cstdio>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "GameStateManagement/ScreenManager.hpp"
#include "Harness/CommandLine.hpp"
#include "Harness/DemoIndex.hpp"
#include "Navigation/HomeScreen.hpp"

namespace CnaExamples {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::GamerServices::GamerServicesComponent;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Top-level application class. All navigation/UI logic lives in the
// ScreenManager component (see GameStateManagement/ and Navigation/); this
// class only wires up the window/graphics device, pushes the first screen, and
// -- when run headlessly -- drives the scripted-input/screenshot harness.
// See plan.md for the overall architecture.
class CnaExamplesGame : public Game {
public:
    explicit CnaExamplesGame(Harness::Options options = {}) : options_(std::move(options)) {
        getContentProperty().setRootDirectoryProperty("Content");

        graphics_ = std::make_unique<GraphicsDeviceManager>(this);
        graphics_->setPreferredBackBufferWidthProperty(960);
        graphics_->setPreferredBackBufferHeightProperty(640);

        // Must be registered before any Microsoft::Xna::Framework::Net or
        // GamerServices API call (the Net area's demos rely on this being
        // present at app start, not per-screen).
        gamerServices_ = std::make_unique<GamerServicesComponent>(*this);
        getComponentsProperty().Add(&*gamerServices_);

        screenManager_ = std::make_unique<GameStateManagement::ScreenManager>(*this);
        getComponentsProperty().Add(&*screenManager_);

        screenManager_->AddScreen(std::make_shared<Navigation::HomeScreen>(), std::nullopt);

        // A run with a frame budget is a scripted/verification run, so it must be
        // driven only by --keys, never by whatever the desktop happens to deliver.
        if (options_.frames > 0) screenManager_->getInput().SetScriptedOnly(true);

        // --demo pushes the requested screen on top of Home rather than replacing
        // it, so Back still lands somewhere sensible and the screen runs in
        // exactly the context it would have when reached by hand.
        if (!options_.demoPath.empty()) {
            const auto index = Harness::FlattenCatalog(Navigation::BuildAreaCatalog());
            std::string error;
            if (const auto* row = Harness::FindDemo(index, options_.demoPath, error)) {
                screenManager_->AddScreen(row->demo->create(), std::nullopt);
            } else {
                std::fprintf(stderr, "--demo: %s\n", error.c_str());
                startupFailed_ = true;
            }
        }
    }

    [[nodiscard]] bool StartupFailed() const { return startupFailed_; }

    [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CnaExamples.CnaExamplesGame";
        return name;
    }

protected:
    void Update(GameTime& gameTime) override {
        Game::Update(gameTime);
        PumpScriptedKeys();
    }

    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::Black);
        Game::Draw(gameTime); // the ScreenManager component does the real drawing

        frame_++;
        if (options_.frames > 0 && frame_ >= options_.frames) {
            // The capture has to happen after this frame's drawing and before
            // Exit(), or the back buffer read back is the previous frame's.
            if (!options_.screenshotPath.empty()) SaveScreenshot(options_.screenshotPath);
            Exit();
        }
    }

private:
    // One scripted action every --key-interval frames, so each menu transition
    // (0.3s on/off in DemoScreen) has time to settle before the next keystroke.
    void PumpScriptedKeys() {
        if (nextKey_ >= (int)options_.keys.size()) return;
        if (frame_ < (nextKey_ + 1) * options_.keyInterval) return;
        screenManager_->getInput().QueueScriptedAction(options_.keys[(std::size_t)nextKey_]);
        nextKey_++;
    }

    // Back-buffer readback rather than an X11 screen grab: capturing the root
    // window under Xvfb returns solid black for a GL surface, so the only
    // reliable capture is the one the framework itself can do.
    void SaveScreenshot(const std::string& path) {
        auto& device = getGraphicsDeviceProperty();
        const auto& viewport = device.getViewportProperty();
        const int width = viewport.getWidthProperty();
        const int height = viewport.getHeightProperty();
        if (width <= 0 || height <= 0) return;

        const Rectangle region(0, 0, width, height);
        const std::size_t count = (std::size_t)width * (std::size_t)height;
        std::vector<Color> pixels(count, Color(0, 0, 0, 0));
        device.GetBackBufferData(&region, pixels.data(), 0, (int)count);

        std::vector<std::uint8_t> rgba(count * 4);
        for (std::size_t i = 0; i < count; ++i) {
            rgba[i * 4 + 0] = (std::uint8_t)pixels[i].getRProperty();
            rgba[i * 4 + 1] = (std::uint8_t)pixels[i].getGProperty();
            rgba[i * 4 + 2] = (std::uint8_t)pixels[i].getBProperty();
            rgba[i * 4 + 3] = (std::uint8_t)pixels[i].getAProperty();
        }
        Texture2D shot = Texture2D::CreateFromPixels(device, width, height, rgba);
        shot.SaveAsPng(path);
        std::printf("[screenshot] %s (%dx%d, frame %d)\n", path.c_str(), width, height, frame_);
    }

    Harness::Options options_;
    bool startupFailed_ = false;
    int frame_ = 0;
    int nextKey_ = 0;
    std::unique_ptr<GraphicsDeviceManager> graphics_;
    std::unique_ptr<GamerServicesComponent> gamerServices_;
    std::unique_ptr<GameStateManagement::ScreenManager> screenManager_;
};

} // namespace CnaExamples
