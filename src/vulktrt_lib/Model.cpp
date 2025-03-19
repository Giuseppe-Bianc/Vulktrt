/*
 * Created by gbian on 15/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-pro-type-member-init, *-member-init, *-avoid-c-arrays,*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay,*-no-array-decay)
// clang-format on
#include "Vulktrt/Model.hpp"

namespace lve {
    static inline constexpr auto modelVertexs = sizeof(Model::Vertex);
    DISABLE_WARNINGS_PUSH(26432)
    Model::Model(Device &devicein, const std::vector<Vertex> &vertices) : lveDevice{devicein} { createVertexBuffer(vertices); }

    Model::~Model() {
        vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
    }
    DISABLE_WARNINGS_POP()

    void Model::createVertexBuffer(const std::vector<Vertex> &vertices) {
        vertexCount = C_UI32T(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer,
                               vertexBufferMemory);

        void *data = nullptr;
        vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), C_ST(bufferSize));
        lveDevice.setObjectName(VK_OBJECT_TYPE_BUFFER, BC_UI64T(vertexBuffer), "Model Vertex Buffer");
        lveDevice.setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, BC_UI64T(vertexBufferMemory), "Model Vertex Buffer Memory");
        vkUnmapMemory(lveDevice.device(), vertexBufferMemory);
    }

    DISABLE_WARNINGS_PUSH(26485)
    void Model::bind(VkCommandBuffer commandBuffer) noexcept {
        VkBuffer buffers[] = {vertexBuffer};
        const VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }
    DISABLE_WARNINGS_POP()

    void Model::draw(VkCommandBuffer commandBuffer) const noexcept { vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0); }
    DISABLE_WARNINGS_PUSH(26446)
    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = modelVertexs;
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }
    DISABLE_WARNINGS_POP()
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-pro-type-member-init, *-member-init, *-avoid-c-arrays,*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay,*-no-array-decay)
// clang-format on
