/*
* Created by gbian on 17/03/2025.
* Copyright (c) 2025 All rights reserved.
*/
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "headers.hpp"
#include "Model.hpp"

namespace lve {
    struct Transform2dComponent {
        glm::vec2 translation{};  // (position offset)
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c, s}, {-s, c}};

            glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
            return rotMatrix * scaleMat;
        }
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

        id_t getId() const { return id; }
        std::shared_ptr<Model> model{};
        glm::vec3 color{};
        Transform2dComponent transform2d{};
    private:
        GameObject(id_t id) : id{id} {}

        id_t id;
    };
} // lve

// NOLINTEND(*-include-cleaner)