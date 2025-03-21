/*
 * Created by gbian on 10/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "Device.hpp"
#include "Model.hpp"
#include "headers.hpp"

namespace lve {
    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo &) = delete;
        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        VkPipelineMultisampleStateCreateInfo multisampleInfo{};
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        std::vector<VkDynamicState> dynamicStateEnables{};
        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline {
    public:
        Pipeline(Device &device, const std::string &vertFilepath, const std::string &fragFilepath, const PipelineConfigInfo &configInfo);
        ~Pipeline();

        Pipeline(const Pipeline &other) = delete;
        Pipeline &operator=(const Pipeline &other) = delete;

        void bind(VkCommandBuffer commandBuffer) noexcept;

        static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

    private:
        static std::vector<char> readFile(const std::string &filepath);

        void createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath, const PipelineConfigInfo &configInfo);

        void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

        Device &lveDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}  // namespace lve

// NOLINTEND(*-include-cleaner)
