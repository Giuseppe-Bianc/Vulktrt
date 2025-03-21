/*
 * Created by gbian on 19/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "headers.hpp"

namespace lve {
    class Camera {
    public:
        Camera() = default;
        ~Camera() = default;

        Camera(const Camera &other) = delete;
        Camera &operator=(const Camera &other) = delete;

        void setOrthographicProjection(float left, float right, float bottom, float top, float near, float far);
        void setPerspectiveProjection(float fov, float aspect, float near, float far);

        void setViewDirection(const glm::vec3 &position, const glm::vec3 &direction, const glm::vec3 &up = glm::vec3{0.f, -1.f, 0.f});
        void setViewTarget(const glm::vec3 &position, const glm::vec3 &target, const glm::vec3 &up = glm::vec3{0.f, -1.f, 0.f});
        void setViewYXZ(const glm::vec3 &position, const glm::vec3 &rotation);

        const glm::mat4 &getProjection() const { return projectionMatrix; }
        const glm::mat4 &getView() const { return viewMatrix; }
        // const glm::mat4 &getProjectionViewMatrix() const { return projectionViewMatrix; }

    private:
        // void updateViewMatrix();
        // void updateProjectionViewMatrix();

        glm::mat4 projectionMatrix{1.0f};
        glm::mat4 viewMatrix{1.0f};
        // glm::mat4 projectionViewMatrix{1.0f};
        // glm::vec3 position{0.0f};
        // glm::vec3 rotation{0.0f};
    };
}  // namespace lve
   // NOLINTEND(*-include-cleaner)