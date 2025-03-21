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
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            bool operator==(const Vertex &other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            void loadModel(const std::string &filename);
        };

        Model(Device &devicein, const Builder &builder);
        ~Model();

        Model(const Model &other) = delete;
        Model &operator=(const Model &other) = delete;

        static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filepath);

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
}  // namespace lve

// NOLINTEND(*-include-cleaner)