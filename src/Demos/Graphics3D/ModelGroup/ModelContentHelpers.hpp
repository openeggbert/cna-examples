// SPDX-License-Identifier: MIT
#pragma once

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

#include "Demos/Graphics3D/Geometry3DHelpers.hpp"

namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Matrix;
using Microsoft::Xna::Framework::Quaternion;
using Microsoft::Xna::Framework::Vector3;

// D3 (Model Content): every fixture these screens load is synthesized at OnDemoLoad time into a
// temp directory, exactly like Audio/Xact's banks and Media/Pictures -- no asset is borrowed from
// ../cna and nothing Ms-PL enters this repository. This is possible (unlike XACT, which genuinely
// has no Linux-side authoring tool) because ContentManager's own Model/.cnj + .skeleton.bin/
// .clip.bin binary formats are plain, fully-documented CNA formats this app can write directly --
// the same approach ../cna's own easygl_model_skinned_animation_playback_test.cpp golden test
// uses to build its fixture in-process.

inline std::string ModelContentDirectory() {
    return (std::filesystem::temp_directory_path() / "cna-examples-modelcontent").string();
}

inline void AppendFloat(std::vector<std::uint8_t>& out, float v) {
    std::uint8_t bytes[4];
    std::memcpy(bytes, &v, 4);
    out.insert(out.end(), bytes, bytes + 4);
}

inline void AppendInt32(std::vector<std::uint8_t>& out, std::int32_t v) {
    std::uint8_t bytes[4];
    std::memcpy(bytes, &v, 4);
    out.insert(out.end(), bytes, bytes + 4);
}

inline void AppendDouble(std::vector<std::uint8_t>& out, double v) {
    std::uint8_t bytes[8];
    std::memcpy(bytes, &v, 8);
    out.insert(out.end(), bytes, bytes + 8);
}

inline void AppendUint16(std::vector<std::uint8_t>& out, std::uint16_t v) {
    std::uint8_t bytes[2];
    std::memcpy(bytes, &v, 2);
    out.insert(out.end(), bytes, bytes + 2);
}

inline void AppendVector3(std::vector<std::uint8_t>& out, const Vector3& v) {
    AppendFloat(out, v.X); AppendFloat(out, v.Y); AppendFloat(out, v.Z);
}

inline void AppendMatrix(std::vector<std::uint8_t>& out, const Matrix& m) {
    AppendFloat(out, m.M11); AppendFloat(out, m.M12); AppendFloat(out, m.M13); AppendFloat(out, m.M14);
    AppendFloat(out, m.M21); AppendFloat(out, m.M22); AppendFloat(out, m.M23); AppendFloat(out, m.M24);
    AppendFloat(out, m.M31); AppendFloat(out, m.M32); AppendFloat(out, m.M33); AppendFloat(out, m.M34);
    AppendFloat(out, m.M41); AppendFloat(out, m.M42); AppendFloat(out, m.M43); AppendFloat(out, m.M44);
}

inline void WriteBinaryFile(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes) {
    std::ofstream f(path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

inline void WriteTextFile(const std::filesystem::path& path, const std::string& text) {
    std::ofstream f(path, std::ios::binary);
    f << text;
}

// A single-pixel QOI texture, the same minimal encoder ../cna's own SkinnedEffect/Model tests use
// (every pixel a literal QOI_OP_RGBA chunk) -- SkinnedEffect's real XNA shader is always textured.
inline void WriteSolidColorQoi(const std::filesystem::path& path, std::uint8_t r, std::uint8_t g,
                               std::uint8_t b, std::uint8_t a) {
    auto appendU32BE = [](std::vector<std::uint8_t>& out, std::uint32_t v) {
        out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFF));
        out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFF));
        out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
        out.push_back(static_cast<std::uint8_t>(v & 0xFF));
    };
    std::vector<std::uint8_t> bytes;
    bytes.push_back('q'); bytes.push_back('o'); bytes.push_back('i'); bytes.push_back('f');
    appendU32BE(bytes, 2); appendU32BE(bytes, 2);
    bytes.push_back(4);
    bytes.push_back(0);
    for (int i = 0; i < 4; ++i) {
        bytes.push_back(0xFF);
        bytes.push_back(r); bytes.push_back(g); bytes.push_back(b); bytes.push_back(a);
    }
    const std::uint8_t padding[8] = {0, 0, 0, 0, 0, 0, 0, 1};
    bytes.insert(bytes.end(), padding, padding + 8);
    WriteBinaryFile(path, bytes);
}

// Raw stride-32 (VertexPositionNormalTexture) vertex bytes + 16-bit index bytes for a quad built
// by Geometry3DHelpers.hpp's own BuildQuadNormalTextureMesh -- ModelTypeReader's "vertexStride":
// 32 branch expects exactly this layout (pos12+normal12+uv8), see
// BuildVertexBufferFromRawBytes()'s stride==32 case in ../cna's ContentManager.cpp.
inline void BuildQuadStride32Bytes(float width, float height, std::vector<std::uint8_t>& outVerts,
                                   std::vector<std::uint8_t>& outIndices, float offsetX = 0.0f) {
    QuadNormalTextureMesh mesh = BuildQuadNormalTextureMesh(width, height);
    // Model::Draw() applies a single shared world matrix to every ModelMeshPart in the Model, so
    // two side-by-side meshes need their separation baked into their own vertex data -- the exact
    // fix ModelGroup/ProceduralModelScreen.hpp's own OnDemoLoad() comment already established.
    for (auto& v : mesh.vertices) v.Position.X += offsetX;
    outVerts.clear();
    for (const auto& v : mesh.vertices) {
        AppendVector3(outVerts, v.Position);
        AppendVector3(outVerts, v.Normal);
        AppendFloat(outVerts, v.TextureCoordinate.X);
        AppendFloat(outVerts, v.TextureCoordinate.Y);
    }
    outIndices.clear();
    for (auto i : mesh.indices) AppendUint16(outIndices, i);
}

// A single skinned quad (stride-52 SkinnedGpuVertex: pos12+normal12+uv8+weights16+indices4), every
// vertex 100% bound to one bone -- the exact layout/binding ../cna's own
// easygl_model_skinned_animation_playback_test.cpp (Task 942) already proved end to end.
inline void BuildSkinnedQuadBytes(float halfWidth, float halfHeight, std::uint8_t boneIndex,
                                  std::vector<std::uint8_t>& outVerts,
                                  std::vector<std::uint8_t>& outIndices) {
    struct SkinnedGpuVertex {
        float px, py, pz;
        float nx, ny, nz;
        float u, v;
        float w0, w1, w2, w3;
        std::uint8_t i0, i1, i2, i3;
    };
    const SkinnedGpuVertex verts[4] = {
        { -halfWidth,  halfHeight, 0,  0, 0, 1,  0, 0,  1, 0, 0, 0,  boneIndex, 0, 0, 0 },
        { -halfWidth, -halfHeight, 0,  0, 0, 1,  0, 1,  1, 0, 0, 0,  boneIndex, 0, 0, 0 },
        {  halfWidth, -halfHeight, 0,  0, 0, 1,  1, 1,  1, 0, 0, 0,  boneIndex, 0, 0, 0 },
        {  halfWidth,  halfHeight, 0,  0, 0, 1,  1, 0,  1, 0, 0, 0,  boneIndex, 0, 0, 0 },
    };
    outVerts.assign(reinterpret_cast<const std::uint8_t*>(verts),
                    reinterpret_cast<const std::uint8_t*>(verts) + sizeof(verts));
    outIndices.clear();
    for (std::uint16_t i : {0, 1, 2, 0, 2, 3}) AppendUint16(outIndices, i);
}

// .skeleton.bin: boneCount(i32), boneCount * parentIndex(i32), boneCount * bindPose matrix,
// boneCount * inverseBindPose matrix. Byte-for-byte what ContentManager.cpp's ModelTypeReader
// reads back (BinReaderEXT), confirmed by reading that function directly.
inline std::vector<std::uint8_t> BuildSkeletonBytes(const std::vector<std::int32_t>& parents,
                                                     const std::vector<Matrix>& bindPose,
                                                     const std::vector<Matrix>& inverseBindPose) {
    std::vector<std::uint8_t> out;
    AppendInt32(out, static_cast<std::int32_t>(parents.size()));
    for (auto p : parents) AppendInt32(out, p);
    for (const auto& m : bindPose) AppendMatrix(out, m);
    for (const auto& m : inverseBindPose) AppendMatrix(out, m);
    return out;
}

struct KeyframeSpec {
    double timeSeconds;
    Vector3 translation;
    Quaternion rotation = Quaternion::Identity;
    Vector3 scale{1.0f, 1.0f, 1.0f};
};

struct TrackSpec {
    std::int32_t boneIndex;
    std::vector<KeyframeSpec> keys;
};

// .clip.bin: duration(double), trackCount(i32), then per track: boneIndex(i32), keyCount(i32),
// then per key: time(double), translation(3f), rotation(4f xyzw), scale(3f).
inline std::vector<std::uint8_t> BuildClipBytes(double durationSeconds, const std::vector<TrackSpec>& tracks) {
    std::vector<std::uint8_t> out;
    AppendDouble(out, durationSeconds);
    AppendInt32(out, static_cast<std::int32_t>(tracks.size()));
    for (const auto& track : tracks) {
        AppendInt32(out, track.boneIndex);
        AppendInt32(out, static_cast<std::int32_t>(track.keys.size()));
        for (const auto& key : track.keys) {
            AppendDouble(out, key.timeSeconds);
            AppendVector3(out, key.translation);
            AppendFloat(out, key.rotation.X); AppendFloat(out, key.rotation.Y);
            AppendFloat(out, key.rotation.Z); AppendFloat(out, key.rotation.W);
            AppendVector3(out, key.scale);
        }
    }
    return out;
}

struct MorphTargetSpec {
    std::vector<Vector3> positionDeltas;
};

// Morph target binary sidecar: targetCount(i32), then per target: vertexCount(i32),
// vertexCount*float32[3] position deltas, hasNormalDeltas(i32, always 0 here -- these demos morph
// position only). Byte-for-byte ../cna's tools/gltf_to_cnj/gltf_to_cnj.cpp's own BuildMorphBytes().
inline std::vector<std::uint8_t> BuildMorphBytes(const std::vector<MorphTargetSpec>& targets) {
    std::vector<std::uint8_t> out;
    AppendInt32(out, static_cast<std::int32_t>(targets.size()));
    for (const auto& target : targets) {
        AppendInt32(out, static_cast<std::int32_t>(target.positionDeltas.size()));
        for (const auto& p : target.positionDeltas) AppendVector3(out, p);
        AppendInt32(out, 0); // hasNormalDeltas
    }
    return out;
}

} // namespace CnaExamples::Demos::Graphics3D::ModelGroupDemos
