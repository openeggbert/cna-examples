// SPDX-License-Identifier: MIT
#pragma once

#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"

#include "GameStateManagement/ScreenManager.hpp"
#include "Navigation/HomeScreen.hpp"

namespace CnaExamples {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::GamerServices::GamerServicesComponent;

// Top-level application class. All navigation/UI logic lives in the
// ScreenManager component (see GameStateManagement/ and Navigation/); this
// class only wires up the window/graphics device and pushes the first
// screen. See plan.md for the overall architecture.
class CnaExamplesGame : public Game {
public:
    CnaExamplesGame() {
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
    }

    [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CnaExamples.CnaExamplesGame";
        return name;
    }

protected:
    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::Black);
        Game::Draw(gameTime); // the ScreenManager component does the real drawing
    }

private:
    std::unique_ptr<GraphicsDeviceManager> graphics_;
    std::unique_ptr<GamerServicesComponent> gamerServices_;
    std::unique_ptr<GameStateManagement::ScreenManager> screenManager_;
};

} // namespace CnaExamples
