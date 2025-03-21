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
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        Model(Device &devicein, const Builder &builder);
        ~Model();

        Model(const Model &other) = delete;
        Model &operator=(const Model &other) = delete;

        void bind(VkCommandBuffer commandBuffer) noexcept;
        void draw(VkCommandBuffer commandBuffer) const noexcept;

        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

    private:
        Device &lveDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        std::uint32_t vertexCount;
        bool hasIndexBuffer{false};
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        std::uint32_t indexCount;
    };
} // namespace lve

// NOLINTEND(*-include-cleaner)