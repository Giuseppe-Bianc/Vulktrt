/*
 * Created by gbian on 15/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once
#include "Buffer.hpp"
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
            void loadModel(const std::string &filepath);
        };

        Model(Device &devicein, const Builder &builder);
        ~Model() = default;

        Model(const Model &other) = delete;
        Model &operator=(const Model &other) = delete;

        static std::unique_ptr<Model> createModelFromFile(Device &device, const std::string &filepath);

        void bind(VkCommandBuffer commandBuffer, const std::string &name) noexcept;
        void draw(VkCommandBuffer commandBuffer) const noexcept;

        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

    private:
        Device &lveDevice;

        std::unique_ptr<Buffer> vertexBuffer;
        std::uint32_t vertexCount;
        bool hasIndexBuffer{false};
        std::unique_ptr<Buffer> indexBuffer;
        std::uint32_t indexCount;
    };
}  // namespace lve

// NOLINTEND(*-include-cleaner)