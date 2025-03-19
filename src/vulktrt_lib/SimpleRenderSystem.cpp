/*
 * Created by gbian on 18/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init)
// clang-format on
#include "Vulktrt/SimpleRenderSystem.hpp"

namespace lve {
    DISABLE_WARNINGS_PUSH(4324)

    struct SimplePushConstantData {
        glm::mat4 transform{1.f};
        alignas(16) glm::vec3 color;
    };

    static inline constexpr auto spcds = sizeof(SimplePushConstantData);
    static inline constexpr auto glm2pi = glm::two_pi<float>();
    DISABLE_WARNINGS_POP()

    SimpleRenderSystem::SimpleRenderSystem(Device &device, VkRenderPass renderPass) : lveDevice{device} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() { vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); }

    void SimpleRenderSystem::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = spcds;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        VK_CHECK(vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
                 "failed to create pipeline layout!");
        lveDevice.setObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, BC_UI64T(pipelineLayout), "Simple Pipeline Layout");
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<Pipeline>(
            lveDevice, calculateRelativePathToShaders(curentP, "simple_shader.vert.opt.rmp.spv").string(),
            calculateRelativePathToShaders(curentP, "simple_shader.frag.opt.rmp.spv").string(), pipelineConfig);
    }

    void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects) {
        lvePipeline->bind(commandBuffer);

        for(auto &obj : gameObjects) {
            obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.0005f, glm2pi);
            obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.00025f, glm2pi);

            SimplePushConstantData push{};
            push.color = obj.color;
            push.transform = obj.transform.mat4();

            vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, spcds, &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init)
// clang-format on
