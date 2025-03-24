/*
 * Created by gbian on 19/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-easily-swappable-parameters, *-uppercase-literal-suffix, *-pro-type-union-access,*-avoid-magic-numbers,*-magic-numbers)
// clang-format on
#include "Vulktrt/Camera.hpp"

namespace lve {

    void Camera::setOrthographicProjection(float left, float right, float bottom, float top, float near, float far) {
        projectionMatrix = glm::orthoLH(left, right, bottom, top, near, far);
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        assert(glm::abs(aspect - fepsilon));
        projectionMatrix = glm::perspectiveLH(fovy, aspect, near, far);
    }

    void Camera::setViewDirection(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &up) {
        glm::vec3 center = position - direction;
        auto upf = glm::vec3{up.x, -up.y, up.z};
        viewMatrix = glm::lookAt(position, center, upf);
    }

    void Camera::setViewTarget(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up) {
        setViewDirection(position, target - position, up);
    }

    void Camera::setViewYXZ(const glm::vec3 &position, const glm::vec3 &rotation) {
        glm::mat4 rotationMatrix = glm::eulerAngleZXY(rotation.z, rotation.x, rotation.y);
        // Set the view matrix using the position and the rotation matrix
        viewMatrix  = glm::translate(glm::mat4(1.0f), -position) * rotationMatrix;
    }
} // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-easily-swappable-parameters, *-uppercase-literal-suffix, *-pro-type-union-access,*-avoid-magic-numbers,*-magic-numbers)
// clang-format on