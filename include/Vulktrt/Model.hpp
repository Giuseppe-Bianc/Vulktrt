/*
 * Created by gbian on 15/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once
#include "Device.hpp"

namespace lve {

    class Model {
    public:
        struct Vertex {
            glm::vec2 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        Model(Device &devicein, const std::vector<Vertex> &vertices);
        ~Model();

        Model(const Model &other) = delete;
        Model &operator=(const Model &other) = delete;

        void bind(VkCommandBuffer commandBuffer) noexcept;
        void draw(VkCommandBuffer commandBuffer) noexcept;

        void createVertexBuffer(const std::vector<Vertex> &vertices);

    private:
        Device &lveDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        std::uint32_t vertexCount;
    };
}  // namespace lve

// NOLINTEND(*-include-cleaner)
