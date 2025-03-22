/*
* Created by gbian on 22/03/2025.
* Copyright (c) 2025 All rights reserved.
*/
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "Camera.hpp"

namespace lve {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera camera;
    };
}

// NOLINTEND(*-include-cleaner)