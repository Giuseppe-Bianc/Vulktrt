/*
 * Created by gbian on 17/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "Model.hpp"
#include "headers.hpp"

namespace lve {
    struct TransformComponent {
        glm::vec3 translation{};  // (position offset)
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation;

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4() const noexcept;
        glm::mat3 normalMatrix() const noexcept;
    };

    class GameObject {
    public:
        using id_t = std::uint32_t;

        static GameObject createGameObject() {
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        GameObject(const GameObject &other) = delete;
        GameObject(GameObject &&other) noexcept = default;
        GameObject &operator=(const GameObject &other) = delete;
        GameObject &operator=(GameObject &&other) noexcept = default;
        ~GameObject() = default;

        id_t getId() const noexcept { return id; }
        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        GameObject(id_t idin) : id{idin} {}

        id_t id;
    };
}  // namespace lve

// NOLINTEND(*-include-cleaner)
