/*
* Created by gbian on 19/03/2025.
* Copyright (c) 2025 All rights reserved.
*/

#include "Vulktrt/Camera.hpp"

namespace lve {
    static inline constexpr auto fepsilon = std::numeric_limits<float>::epsilon();
    void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
        projectionMatrix = glm::ortho(left, right, bottom, top, near, far);
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        assert(glm::abs(aspect - fepsilon) > 0.0f);
        projectionMatrix = glm::perspective(fovy, aspect, near, far);
    }
} // lve