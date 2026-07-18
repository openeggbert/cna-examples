// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"

namespace CnaExamples::Demos::Graphics3D {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Graphics::VertexPositionColor;
using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTexture;
using Microsoft::Xna::Framework::Graphics::VertexPositionTexture;

// Every 3D demo in this Area builds its own geometry procedurally -- no
// bundled model/mesh asset -- mirroring 2D Graphics' TextureDemoHelpers.hpp.
// Cubes are built "hard-edged" (24 vertices: 4 per face, not 8 shared) so
// each face gets its own flat normal/UV, matching XNA's own
// GeometricPrimitive sample convention. Winding is CCW as seen from outside
// each face; most demos should pair this with RasterizerState::CullNone (so
// winding mistakes are never visible as missing geometry) and only the
// dedicated CullMode demo should rely on the winding being exactly this.

struct CubeColorMesh {
    std::vector<VertexPositionColor> vertices;
    std::vector<std::uint16_t> indices;
};

struct CubeTextureMesh {
    std::vector<VertexPositionTexture> vertices;
    std::vector<std::uint16_t> indices;
};

struct CubeNormalTextureMesh {
    std::vector<VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
};

struct QuadNormalTextureMesh {
    std::vector<VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
};

namespace detail {

// One face's 4 corners, built from a normal axis and its two tangent axes,
// wound CCW as seen looking down -normal (i.e. from outside the cube).
inline void AppendFace(std::vector<Vector3>& positions, std::vector<Vector3>& normals,
                       std::vector<Vector2>& uvs, const Vector3& normal,
                       const Vector3& tangentU, const Vector3& tangentV, float halfSize) {
    const Vector3 center = normal * halfSize;
    positions.push_back(center - tangentU * halfSize - tangentV * halfSize);
    positions.push_back(center + tangentU * halfSize - tangentV * halfSize);
    positions.push_back(center + tangentU * halfSize + tangentV * halfSize);
    positions.push_back(center - tangentU * halfSize + tangentV * halfSize);
    for (int i = 0; i < 4; ++i) normals.push_back(normal);
    uvs.push_back(Vector2(0.0f, 1.0f));
    uvs.push_back(Vector2(1.0f, 1.0f));
    uvs.push_back(Vector2(1.0f, 0.0f));
    uvs.push_back(Vector2(0.0f, 0.0f));
}

inline void AppendFaceIndices(std::vector<std::uint16_t>& indices, std::uint16_t base) {
    indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
    indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
}

// The 6 faces in a fixed order: +X, -X, +Y, -Y, +Z, -Z.
inline void BuildCubeFaces(float halfSize, std::vector<Vector3>& positions,
                           std::vector<Vector3>& normals, std::vector<Vector2>& uvs,
                           std::vector<std::uint16_t>& indices) {
    AppendFace(positions, normals, uvs, Vector3(1, 0, 0), Vector3(0, 0, -1), Vector3(0, 1, 0), halfSize);
    AppendFace(positions, normals, uvs, Vector3(-1, 0, 0), Vector3(0, 0, 1), Vector3(0, 1, 0), halfSize);
    AppendFace(positions, normals, uvs, Vector3(0, 1, 0), Vector3(1, 0, 0), Vector3(0, 0, -1), halfSize);
    AppendFace(positions, normals, uvs, Vector3(0, -1, 0), Vector3(1, 0, 0), Vector3(0, 0, 1), halfSize);
    AppendFace(positions, normals, uvs, Vector3(0, 0, 1), Vector3(1, 0, 0), Vector3(0, 1, 0), halfSize);
    AppendFace(positions, normals, uvs, Vector3(0, 0, -1), Vector3(-1, 0, 0), Vector3(0, 1, 0), halfSize);
    for (int face = 0; face < 6; ++face) AppendFaceIndices(indices, (std::uint16_t)(face * 4));
}

} // namespace detail

// A cube with one flat color per face (faceColors[0..5] = +X,-X,+Y,-Y,+Z,-Z).
inline CubeColorMesh BuildCubeColorMesh(float halfSize, const std::array<Color, 6>& faceColors) {
    std::vector<Vector3> positions, normals;
    std::vector<Vector2> uvs;
    CubeColorMesh mesh;
    detail::BuildCubeFaces(halfSize, positions, normals, uvs, mesh.indices);
    mesh.vertices.reserve(positions.size());
    for (std::size_t i = 0; i < positions.size(); ++i) {
        mesh.vertices.emplace_back(positions[i], faceColors[i / 4]);
    }
    return mesh;
}

// A cube with each face UV-mapped 0..1 (same texture repeated per face).
inline CubeTextureMesh BuildCubeTextureMesh(float halfSize) {
    std::vector<Vector3> positions, normals;
    std::vector<Vector2> uvs;
    CubeTextureMesh mesh;
    detail::BuildCubeFaces(halfSize, positions, normals, uvs, mesh.indices);
    mesh.vertices.reserve(positions.size());
    for (std::size_t i = 0; i < positions.size(); ++i) {
        mesh.vertices.emplace_back(positions[i], uvs[i]);
    }
    return mesh;
}

// A cube with per-face normals and UVs -- the lit + textured path
// (BasicEffect's confirmed real lit-textured combination requires this
// vertex type; VertexPositionColor has no lit shader combination).
inline CubeNormalTextureMesh BuildCubeNormalTextureMesh(float halfSize) {
    std::vector<Vector3> positions, normals;
    std::vector<Vector2> uvs;
    CubeNormalTextureMesh mesh;
    detail::BuildCubeFaces(halfSize, positions, normals, uvs, mesh.indices);
    mesh.vertices.reserve(positions.size());
    for (std::size_t i = 0; i < positions.size(); ++i) {
        mesh.vertices.emplace_back(positions[i], normals[i], uvs[i]);
    }
    return mesh;
}

// A single flat quad facing +Z, centered at the origin, in the XY plane --
// for single-surface effect demos (AlphaTest/DualTexture/EnvironmentMap)
// that don't need a full cube to make their point.
inline QuadNormalTextureMesh BuildQuadNormalTextureMesh(float width, float height) {
    QuadNormalTextureMesh mesh;
    const float hw = width / 2.0f;
    const float hh = height / 2.0f;
    mesh.vertices.emplace_back(Vector3(-hw, -hh, 0.0f), Vector3(0, 0, 1), Vector2(0.0f, 1.0f));
    mesh.vertices.emplace_back(Vector3(hw, -hh, 0.0f), Vector3(0, 0, 1), Vector2(1.0f, 1.0f));
    mesh.vertices.emplace_back(Vector3(hw, hh, 0.0f), Vector3(0, 0, 1), Vector2(1.0f, 0.0f));
    mesh.vertices.emplace_back(Vector3(-hw, hh, 0.0f), Vector3(0, 0, 1), Vector2(0.0f, 0.0f));
    mesh.indices = {0, 1, 2, 0, 2, 3};
    return mesh;
}

} // namespace CnaExamples::Demos::Graphics3D
