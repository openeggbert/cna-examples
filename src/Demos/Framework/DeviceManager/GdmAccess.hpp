// SPDX-License-Identifier: MIT
#pragma once

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameServiceContainer.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/IGraphicsDeviceManager.hpp"

namespace CnaExamples::Demos::Framework::DeviceManagerDemos {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::IGraphicsDeviceManager;

// The GraphicsDeviceManager is owned privately by the application's Game
// subclass, but it registers itself in Game::Services as IGraphicsDeviceManager
// (and as IGraphicsDeviceService) when it is created. Looking it up through the
// container is therefore the XNA-idiomatic way for unrelated code to reach it,
// and it is what these demos do rather than requiring the app to hand out a
// pointer.
//
// Returns nullptr if no manager is registered, which every caller must handle:
// a Game is not required to have one at all.
inline GraphicsDeviceManager* FindDeviceManager(Game& game) {
    return dynamic_cast<GraphicsDeviceManager*>(
        game.getServicesProperty().GetService<IGraphicsDeviceManager>());
}

} // namespace CnaExamples::Demos::Framework::DeviceManagerDemos
