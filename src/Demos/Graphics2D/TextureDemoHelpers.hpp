// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace CnaExamples::Demos::Graphics2D {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Every demo in this Area needs no bundled PNG asset -- textures are built
// procedurally at runtime via Texture2D::CreateFromPixels(), mirroring the
// Audio area's GenerateSineWavePcm16(). All three helpers write straight
// RGBA8 bytes, matching CreateFromPixels()'s expected layout.

inline std::vector<std::uint8_t> MakePixelBuffer(int width, int height) {
    return std::vector<std::uint8_t>((std::size_t)width * (std::size_t)height * 4);
}

inline void SetPixel(std::vector<std::uint8_t>& pixels, int width, int x, int y, Color c) {
    const std::size_t i = ((std::size_t)y * (std::size_t)width + (std::size_t)x) * 4;
    pixels[i + 0] = c.getRProperty();
    pixels[i + 1] = c.getGProperty();
    pixels[i + 2] = c.getBProperty();
    pixels[i + 3] = c.getAProperty();
}

// A 2-color checkerboard, cellSize pixels per square.
inline Texture2D CreateCheckerboardTexture(GraphicsDevice& device, int width, int height,
                                            int cellSize, Color colorA, Color colorB) {
    auto pixels = MakePixelBuffer(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const bool isA = ((x / cellSize) + (y / cellSize)) % 2 == 0;
            SetPixel(pixels, width, x, y, isA ? colorA : colorB);
        }
    }
    return Texture2D::CreateFromPixels(device, width, height, pixels);
}

// A left-to-right linear gradient between two colors.
inline Texture2D CreateGradientTexture(GraphicsDevice& device, int width, int height,
                                        Color left, Color right) {
    auto pixels = MakePixelBuffer(width, height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const float t = width > 1 ? (float)x / (float)(width - 1) : 0.0f;
            const Color c((std::uint8_t)(left.getRProperty() + t * (right.getRProperty() - left.getRProperty())),
                           (std::uint8_t)(left.getGProperty() + t * (right.getGProperty() - left.getGProperty())),
                           (std::uint8_t)(left.getBProperty() + t * (right.getBProperty() - left.getBProperty())),
                           (std::uint8_t)(left.getAProperty() + t * (right.getAProperty() - left.getAProperty())));
            SetPixel(pixels, width, x, y, c);
        }
    }
    return Texture2D::CreateFromPixels(device, width, height, pixels);
}

// A grid of distinctly-colored cells (cols x rows), each cellSize pixels
// square -- a sprite-sheet-shaped atlas for source-rectangle-cropping demos.
// `palette` is indexed by (row * cols + col) % palette.size().
inline Texture2D CreateGridTexture(GraphicsDevice& device, int cols, int rows, int cellSize,
                                    const std::vector<Color>& palette) {
    const int width = cols * cellSize;
    const int height = rows * cellSize;
    auto pixels = MakePixelBuffer(width, height);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            const Color& c = palette[(std::size_t)(row * cols + col) % palette.size()];
            for (int y = row * cellSize; y < (row + 1) * cellSize; ++y) {
                for (int x = col * cellSize; x < (col + 1) * cellSize; ++x) {
                    SetPixel(pixels, width, x, y, c);
                }
            }
        }
    }
    return Texture2D::CreateFromPixels(device, width, height, pixels);
}

} // namespace CnaExamples::Demos::Graphics2D
