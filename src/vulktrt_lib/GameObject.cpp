/*
 * Created by gbian on 17/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner, *-uppercase-literal-suffix, *-pro-type-union-access)
#include "Vulktrt/GameObject.hpp"

namespace lve {
    glm::mat4 TransformComponent::mat4() const noexcept {
        glm::mat4 model(1.0f);
        // Apply translation
        model = glm::translate(glm::mat4(1.0f), translation);
        // Apply rotation (using Euler angles in YXZ order)
        model *= glm::eulerAngleYXZ(rotation.x, rotation.y, rotation.z);
        // Apply scaling
        model = glm::scale(model, scale);
        return model;
    }

    glm::mat3 TransformComponent::normalMatrix() noexcept {
        return glm::mat3(glm::inverseTranspose(mat4()));
    }
} // namespace lve

// NOLINTEND(*-include-cleaner, *-uppercase-literal-suffix, *-pro-type-union-access)