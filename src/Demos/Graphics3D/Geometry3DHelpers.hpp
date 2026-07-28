// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTangentTexture.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionNormalTexture.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"

namespace CnaExamples::Demos::Graphics3D {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Vector3;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Graphics::VertexPositionColor;
using Microsoft::Xna::Framework::Graphics::VertexPositionNormalTangentTexture;
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

// A UV sphere carrying tangents -- the vertex layout PbrEffect's normal
// mapping needs. A cube is the wrong shape for PBR: roughness and metallic
// read as a specular highlight sweeping across a curved surface, and a flat
// face shows almost none of that.
//
// The tangent points along +U (increasing longitude), which is the convention
// tangent-space normal maps are authored against. Getting it backwards does
// not fail loudly -- it lights the surface from the wrong side.
//
// Tangent is a Vector4, not a Vector3: W carries the bitangent's HANDEDNESS,
// following glTF's own TANGENT accessor convention
//     Bitangent = cross(Normal, Tangent.xyz) * Tangent.W
// For this sphere cross(N, T) works out to exactly d(position)/d(phi), which
// points along +V, so W is +1. Deriving that is worth the trouble: a wrong
// sign flips the bitangent and mirrors every normal map along V, which looks
// like subtly bad lighting rather than an obvious error.
struct SphereTangentMesh {
    std::vector<VertexPositionNormalTangentTexture> vertices;
    std::vector<std::uint16_t> indices;
};

inline SphereTangentMesh BuildSphereTangentMesh(float radius, int slices, int stacks) {
    SphereTangentMesh mesh;
    constexpr float kPi = 3.14159265358979323846f;

    for (int stack = 0; stack <= stacks; ++stack) {
        const float v = (float)stack / (float)stacks;
        const float phi = v * kPi;                 // 0 at +Y pole, pi at -Y pole
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi);

        for (int slice = 0; slice <= slices; ++slice) {
            const float u = (float)slice / (float)slices;
            const float theta = u * 2.0f * kPi;
            const float sinTheta = std::sin(theta);
            const float cosTheta = std::cos(theta);

            const Vector3 normal(sinPhi * cosTheta, cosPhi, sinPhi * sinTheta);
            // d(position)/d(theta), normalised: points along increasing U.
            const Vector4 tangent(-sinTheta, 0.0f, cosTheta, 1.0f);
            mesh.vertices.emplace_back(
                Vector3(normal.X * radius, normal.Y * radius, normal.Z * radius),
                normal, tangent, Vector2(u, v));
        }
    }

    const int ring = slices + 1;
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            const std::uint16_t a = (std::uint16_t)(stack * ring + slice);
            const std::uint16_t b = (std::uint16_t)(a + ring);
            // Wound to match the cube helpers above, so the same
            // RasterizerState::CullCounterClockwise every other 3D demo uses
            // keeps the outside faces. The opposite winding culls the front
            // faces and leaves an apparently empty viewport.
            mesh.indices.insert(mesh.indices.end(),
                                {a, b, (std::uint16_t)(a + 1),
                                 (std::uint16_t)(a + 1), b, (std::uint16_t)(b + 1)});
        }
    }
    return mesh;
}

} // namespace CnaExamples::Demos::Graphics3D
