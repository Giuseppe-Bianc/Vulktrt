/*
 * Created by gbian on 15/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-pro-type-member-init, *-member-init, *-avoid-c-arrays,*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay,*-no-array-decay)
// clang-format on
#include "Vulktrt/Model.hpp"
#include "Vulktrt/utils.hpp"

namespace std {
    template <> struct hash<lve::Model::Vertex> {
        size_t operator()(lve::Model::Vertex const &vertex) const {
            size_t seed = 0;
            lve::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
} // namespace std

namespace lve {
    static inline constexpr glm::vec3 DEFAULT_COLOR = {1.f, 1.f, 1.f};
    static inline constexpr auto modelVertexs = sizeof(Model::Vertex);
    DISABLE_WARNINGS_PUSH(26432)

    void Model::Builder::loadModel(const std::string &filepath) {
        vnd::AutoTimer timer(FORMAT("loadModel {}", filepath));
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) { throw std::runtime_error(warn + err); }

        size_t estimatedSize = attrib.vertices.size() / 3;
        vertices.clear();
        indices.clear();
        vertices.reserve(estimatedSize);
        indices.reserve(attrib.vertices.size());

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for(const auto &shape : shapes) {
            for(const auto &[vertex_index, normal_index, texcoord_index] : shape.mesh.indices) {
                Vertex vertex{};

                if(vertex_index >= 0) {
                    vertex.position = {
                        attrib.vertices[3 * vertex_index + 0],
                        attrib.vertices[3 * vertex_index + 1],
                        attrib.vertices[3 * vertex_index + 2],
                    };

                    vertex.color = {
                        attrib.colors[3 * vertex_index + 0],
                        attrib.colors[3 * vertex_index + 1],
                        attrib.colors[3 * vertex_index + 2],
                    };
                }

                if(normal_index >= 0) {
                    vertex.normal = {
                        attrib.normals[3 * normal_index + 0],
                        attrib.normals[3 * normal_index + 1],
                        attrib.normals[3 * normal_index + 2],
                    };
                }

                if(texcoord_index >= 0) {
                    vertex.uv = {
                        attrib.texcoords[2 * texcoord_index + 0],
                        attrib.texcoords[2 * texcoord_index + 1],
                    };
                }

                if(!uniqueVertices.contains(vertex)) {
                    uniqueVertices[vertex] = C_UI32T(vertices.size());
                    vertices.emplace_back(vertex);
                }
                indices.emplace_back(uniqueVertices[vertex]);
            }
        }
    }

    Model::Model(Device &devicein, const Builder &builder)
        : lveDevice{devicein} {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    Model::~Model() {
        const auto deviceDevice = lveDevice.device();
        vkDestroyBuffer(deviceDevice, vertexBuffer, nullptr);
        vkFreeMemory(deviceDevice, vertexBufferMemory, nullptr);
        if(hasIndexBuffer) {
            vkDestroyBuffer(deviceDevice, indexBuffer, nullptr);
            vkFreeMemory(deviceDevice, indexBufferMemory, nullptr);
        }
    }

    DISABLE_WARNINGS_POP()

    void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
        const auto deviceDevice = lveDevice.device();
        vertexCount = C_UI32T(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                               stagingBufferMemory);

        void *data;
        vkMapMemory(deviceDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), C_ST(bufferSize));
        vkUnmapMemory(deviceDevice, stagingBufferMemory);

        lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

        lveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
        lveDevice.setObjectName(VK_OBJECT_TYPE_BUFFER, BC_UI64T(vertexBuffer), "Model Vertex Buffer");
        lveDevice.setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, BC_UI64T(vertexBufferMemory), "Model Vertex Buffer Memory");
        vkDestroyBuffer(deviceDevice, stagingBuffer, nullptr);
        vkFreeMemory(deviceDevice, stagingBufferMemory, nullptr);
    }

    void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
        const auto deviceDevice = lveDevice.device();
        indexCount = C_UI32T(indices.size());
        hasIndexBuffer = indexCount > 0;

        if(!hasIndexBuffer) { return; }

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                               stagingBufferMemory);

        void *data;
        vkMapMemory(deviceDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), C_ST(bufferSize));
        vkUnmapMemory(deviceDevice, stagingBufferMemory);

        lveDevice.createBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
        lveDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);
        lveDevice.setObjectName(VK_OBJECT_TYPE_BUFFER, BC_UI64T(indexBuffer), "Model Index Buffer");
        lveDevice.setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, BC_UI64T(indexBufferMemory), "Model Index Buffer Memory");

        vkDestroyBuffer(deviceDevice, stagingBuffer, nullptr);
        vkFreeMemory(deviceDevice, stagingBufferMemory, nullptr);
    }

    DISABLE_WARNINGS_PUSH(26485)

    std::unique_ptr<Model> Model::createModelFromFile(Device &device, const std::string &filepath) {
        Builder builder{};
        builder.loadModel(filepath);
        // LINFO("Model loaded from file: {}", filepath);
        LINFO("Model has {} vertices and {} indices", builder.vertices.size(), builder.indices.size());
        return std::make_unique<Model>(device, builder);
    }

    void Model::bind(VkCommandBuffer commandBuffer) noexcept {
        VkBuffer buffers[] = {vertexBuffer};
        const VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if(hasIndexBuffer) { vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32); }
    }

    DISABLE_WARNINGS_POP()

    void Model::draw(VkCommandBuffer commandBuffer) const noexcept {
        if(hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    DISABLE_WARNINGS_PUSH(26446)

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = modelVertexs;
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
        attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
        attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});
        return attributeDescriptions;
    }

    DISABLE_WARNINGS_POP()
} // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-pro-type-member-init, *-member-init, *-avoid-c-arrays,*-avoid-c-arrays, *-pro-bounds-array-to-pointer-decay,*-no-array-decay)
// clang-format on
