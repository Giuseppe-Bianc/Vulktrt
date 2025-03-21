/*
 * Created by gbian on 19/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#include "Vulktrt/Camera.hpp"

namespace lve {
    static inline constexpr auto fepsilon = std::numeric_limits<float>::epsilon();

    void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
        projectionMatrix = glm::mat4{1.0f};
        projectionMatrix[0][0] = 2.f / (right - left);
        projectionMatrix[1][1] = 2.f / (bottom - top);
        projectionMatrix[2][2] = 1.f / (far - near);
        projectionMatrix[3][0] = -(right + left) / (right - left);
        projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
        projectionMatrix[3][2] = -near / (far - near);
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        assert(glm::abs(aspect - fepsilon) > 0.0f);
        const float tanHalfFovy = tan(fovy / 2.f);
        const auto range = far - near;
        projectionMatrix = glm::mat4{0.0f};
        projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
        projectionMatrix[1][1] = 1.f / (tanHalfFovy);
        projectionMatrix[2][2] = far / range;
        projectionMatrix[2][3] = 1.f;
        projectionMatrix[3][2] = -(far * near) / range;
    }

    void Camera::setViewDirection(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &up) {
        // The 'center' is the point the camera is looking at.
        glm::vec3 center = position - direction;
        auto upf = glm::vec3{up.x, -up.y, up.z};

        // Generate the view matrix using glm::lookAt
        viewMatrix = glm::lookAt(position, center, upf);
    }

    void Camera::setViewTarget(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up) {
        setViewDirection(position, target - position, up);
    }

    void Camera::setViewYXZ(const glm::vec3 &position, const glm::vec3 &rotation) {
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
        const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
        const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
        viewMatrix = glm::mat4{1.f};
        viewMatrix[0][0] = u.x;
        viewMatrix[1][0] = u.y;
        viewMatrix[2][0] = u.z;
        viewMatrix[0][1] = v.x;
        viewMatrix[1][1] = v.y;
        viewMatrix[2][1] = v.z;
        viewMatrix[0][2] = w.x;
        viewMatrix[1][2] = w.y;
        viewMatrix[2][2] = w.z;
        viewMatrix[3][0] = -glm::dot(u, position);
        viewMatrix[3][1] = -glm::dot(v, position);
        viewMatrix[3][2] = -glm::dot(w, position);
    }
}  // namespace lve

// NOLINTEND(*-include-cleaner)