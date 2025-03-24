/*
 * Created by gbian on 10/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-signed-bitwise, *-pro-type-reinterpret-cast, *-uppercase-literal-suffix,*-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto)
// clang-format on
#include "Vulktrt/Pipeline.hpp"

namespace lve {
    DISABLE_WARNINGS_PUSH(26432)

    Pipeline::Pipeline(Device &device, const std::string &vertFilepath, const std::string &fragFilepath,
                       const PipelineConfigInfo &configInfo)
      : lveDevice{device} {
        createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
    }

    Pipeline::~Pipeline() {
        auto dev = lveDevice.device();
        vkDestroyShaderModule(dev, vertShaderModule, nullptr);
        vkDestroyShaderModule(dev, fragShaderModule, nullptr);
        vkDestroyPipeline(dev, graphicsPipeline, nullptr);
    }

    DISABLE_WARNINGS_POP()

    // Marked as a simple helper function with minimal state dependency.
    std::vector<char> Pipeline::readFile(const std::string &filepath) {
        auto path = fs::path(filepath).lexically_normal();
        std::ifstream file{filepath, std::ios::binary};

        // Use std::filesystem to determine file size.
        const auto fileSize = std::filesystem::file_size(path);
        std::vector<char> buffer(fileSize);

        if(!file) { throw std::runtime_error("failed to open file: " + path.string()); }
        if(!file.read(buffer.data(), C_L(fileSize))) { throw std::runtime_error("failed to read file: " + path.string()); }

        return buffer;
    }

    DISABLE_WARNINGS_PUSH(26446)

    void Pipeline::createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath,
                                          const PipelineConfigInfo &configInfo) {
        assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
        assert(configInfo.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");
        const auto vertCode = readFile(vertFilepath);
        const auto fragCode = readFile(fragFilepath);

        createShaderModule(vertCode, &vertShaderModule);
        createShaderModule(fragCode, &fragShaderModule);

        lveDevice.setObjectName(VK_OBJECT_TYPE_SHADER_MODULE, BC_UI64T(vertShaderModule), "Vertex Shader Module");
        lveDevice.setObjectName(VK_OBJECT_TYPE_SHADER_MODULE, BC_UI64T(fragShaderModule), "Fragment Shader Module");

        // Use std::array to avoid dynamic allocation overhead for a fixed number of shader stages.
        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
        shaderStages[0] = VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                          .stage = VK_SHADER_STAGE_VERTEX_BIT,
                                                          .module = vertShaderModule,
                                                          .pName = "main"};
        shaderStages[1] = VkPipelineShaderStageCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                          .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                                                          .module = fragShaderModule,
                                                          .pName = "main"};

        // Retrieve binding and attribute descriptions once.
        auto bindingDescriptions = Model::Vertex::getBindingDescriptions();
        auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                                                             .vertexBindingDescriptionCount = C_UI32T(bindingDescriptions.size()),
                                                             .pVertexBindingDescriptions = bindingDescriptions.data(),
                                                             .vertexAttributeDescriptionCount = C_UI32T(attributeDescriptions.size()),
                                                             .pVertexAttributeDescriptions = attributeDescriptions.data()};

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = C_UI32T(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.viewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
        pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
        pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
        pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;

        pipelineInfo.layout = configInfo.pipelineLayout;
        pipelineInfo.renderPass = configInfo.renderPass;
        pipelineInfo.subpass = configInfo.subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        VK_CHECK(vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline),
                 "failed to create graphics pipeline");
        lveDevice.setObjectName(VK_OBJECT_TYPE_PIPELINE, BC_UI64T(graphicsPipeline), "Main Graphics Pipeline");
    }

    DISABLE_WARNINGS_POP()

    void Pipeline::createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) {
        VkShaderModuleCreateInfo createInfo{.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                            .codeSize = code.size(),
                                            .pCode = std::bit_cast<const uint32_t *>(code.data())};

        VK_CHECK(vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule), "failed to create shader module!");
    }

    void Pipeline::bind(VkCommandBuffer commandBuffer) noexcept {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }

    void Pipeline::defaultPipelineConfigInfo(PipelineConfigInfo &configInfo) {
        configInfo.inputAssemblyInfo = VkPipelineInputAssemblyStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE};

        configInfo.viewportInfo = VkPipelineViewportStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                                                                    .viewportCount = 1,
                                                                    .pViewports = nullptr,
                                                                    .scissorCount = 1,
                                                                    .pScissors = nullptr};

        configInfo.rasterizationInfo = VkPipelineRasterizationStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f};

        configInfo.multisampleInfo = VkPipelineMultisampleStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                                                                          .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                                                                          .sampleShadingEnable = VK_FALSE,
                                                                          .minSampleShading = 1.0f,
                                                                          .pSampleMask = nullptr,
                                                                          .alphaToCoverageEnable = VK_FALSE,
                                                                          .alphaToOneEnable = VK_FALSE};

        configInfo.colorBlendAttachment = VkPipelineColorBlendAttachmentState{
            .blendEnable = VK_FALSE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT};

        configInfo.colorBlendInfo = VkPipelineColorBlendStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                                                                        .logicOpEnable = VK_FALSE,
                                                                        .logicOp = VK_LOGIC_OP_COPY,
                                                                        .attachmentCount = 1,
                                                                        .pAttachments = &configInfo.colorBlendAttachment,
                                                                        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}};

        configInfo.depthStencilInfo = VkPipelineDepthStencilStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f,
        };

        configInfo.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        configInfo.dynamicStateInfo = VkPipelineDynamicStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .flags = 0,
            .dynamicStateCount = C_UI32T(configInfo.dynamicStateEnables.size()),
            .pDynamicStates = configInfo.dynamicStateEnables.data(),
        };
    }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-signed-bitwise, *-pro-type-reinterpret-cast, *-uppercase-literal-suffix,*-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto)
// clang-format on