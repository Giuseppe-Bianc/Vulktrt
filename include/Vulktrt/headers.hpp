/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */

#pragma once

#include <vulktrt_lib_core/vulktrt_lib_core.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

static inline constexpr std::string_view wtile = Vulktrt::cmake::project_name;
static inline constexpr std::size_t factor = 80;
static inline constexpr auto wfactor = 16;
static inline constexpr auto hfactor = 9;
static inline constexpr auto wwidth = wfactor * factor;
static inline constexpr auto wheight = hfactor * factor;
static inline constexpr auto aspectRatio = C_D(wfactor) / C_D(hfactor);
static inline constexpr auto aspectRatiof = C_F(wfactor) / C_F(hfactor);
static inline constexpr auto ui32tmax = C_UI32T(std::numeric_limits<uint32_t>::max());
static inline constexpr auto uint64Max = C_UI64T(std::numeric_limits<uint64_t>::max());
static inline const auto curentP = fs::current_path();

[[nodiscard]] static constexpr auto calcolaCentro(const int &width, const int &w) noexcept { return (width - w) / 2; }
#define CALC_CENTRO(width, w) calcolaCentro(width, w)

inline fs::path calculateRelativePathToSrc(const fs::path &executablePath, const fs::path &targetFile, const std::string &subDir) {
    fs::path parentDir = executablePath.parent_path();
    while(!fs::exists(parentDir / "src")) {
        parentDir = parentDir.parent_path();
        if(parentDir == parentDir.root_path()) {
            LERROR("Error: 'src' directory not found in the path.");
            return {};
        }
    }
    parentDir = parentDir.parent_path();
    const auto resp = fs::path(subDir);
    const auto relativePathToTarget = parentDir / resp / targetFile;
    const auto relativePath = fs::relative(relativePathToTarget, executablePath);
    return relativePath.lexically_normal();
}

inline fs::path calculateRelativePathToShaders(const fs::path &executablePath, const fs::path &targetFile) {
    return calculateRelativePathToSrc(executablePath, targetFile, "shaders");
}
inline fs::path calculateRelativePathToTextures(const fs::path &executablePath, const fs::path &targetFile) {
    return calculateRelativePathToSrc(executablePath, targetFile, "textures");
}