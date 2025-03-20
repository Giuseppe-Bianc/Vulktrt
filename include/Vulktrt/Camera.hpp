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

        void setPosition(float x, float y, float z);
        void setRotation(float x, float y, float z);

        const glm::mat4 &getProjection() const { return projectionMatrix; }
        // const glm::mat4 &getViewMatrix() const { return viewMatrix; }
        // const glm::mat4 &getProjectionViewMatrix() const { return projectionViewMatrix; }

    private:
        // void updateViewMatrix();
        // void updateProjectionViewMatrix();

        glm::mat4 projectionMatrix{1.0f};
        // glm::mat4 viewMatrix{1.0f};
        // glm::mat4 projectionViewMatrix{1.0f};
        // glm::vec3 position{0.0f};
        // glm::vec3 rotation{0.0f};
    };
}  // namespace lve
   // NOLINTEND(*-include-cleaner)