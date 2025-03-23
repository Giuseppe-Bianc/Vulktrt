/*
 * Created by gbian on 17/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner, *-uppercase-literal-suffix, *-pro-type-union-access)
#include "Vulktrt/GameObject.hpp"

namespace lve {
    glm::mat4 TransformComponent::mat4() const noexcept {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 rotationMatrix = glm::eulerAngleYXZ(rotation.x, rotation.y, rotation.z);
        return transform * rotationMatrix * glm::scale(glm::mat4(1.0f), scale);
    }
    glm::mat3 TransformComponent::normalMatrix() const noexcept { return glm::mat3(glm::transpose(glm::inverse(mat4()))); }
}  // namespace lve

// NOLINTEND(*-include-cleaner, *-uppercase-literal-suffix, *-pro-type-union-access)