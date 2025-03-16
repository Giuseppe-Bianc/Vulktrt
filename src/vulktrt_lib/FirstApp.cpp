/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-fotmat off
// NOLINTBEGIN(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix,
// *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init)
// clang-format on
#include "Vulktrt/FirstApp.hpp"

#include <Vulktrt/FPSCounter.hpp>

namespace lve {
    FirstApp::FirstApp() {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() { vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr); }

    void FirstApp::run() {
        FPSCounter fpsCounter{lveWindow.getGLFWWindow(), wtile};
        while(!lveWindow.shouldClose()) [[likely]] {
            fpsCounter.frameInTitle(false, false);
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadModels() {
        std::vector<Model::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
        lveModel = std::make_unique<Model>(lveDevice, vertices);
    }

    void FirstApp::createPipelineLayout() {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;     // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr;  // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        VK_CHECK(vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout),
                 "failed to create pipeline layout!");
    }

    void FirstApp::recreateSwapChain() {
        auto extent = lveWindow.getExtent();
        while(extent.width == 0 || extent.height == 0) {
            extent = lveWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(lveDevice.device());

        if(lveSwapChain == nullptr) {
            lveSwapChain = std::make_unique<SwapChain>(lveDevice, extent);
        } else {
            lveSwapChain = std::make_unique<SwapChain>(lveDevice, extent, std::move(lveSwapChain));
            if(lveSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

    void FirstApp::createPipeline() {
        assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = lveSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<Pipeline>(
            lveDevice, calculateRelativePathToShaders(curentP, "simple_shader.vert.opt.rmp.spv").string(),
            calculateRelativePathToShaders(curentP, "simple_shader.frag.opt.rmp.spv").string(), pipelineConfig);
    }

    void FirstApp::createCommandBuffers() {
        commandBuffers.resize(lveSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = C_UI32T(commandBuffers.size());

        VK_CHECK(vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()), "failed to allocate command buffers!");
    }

    void FirstApp::freeCommandBuffers() {
        vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), C_UI32T(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VK_CHECK(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo), "failed to begin recording command buffer!");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = C_UI32T(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = C_F(lveSwapChain->getSwapChainExtent().width);
        viewport.height = C_F(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        lvePipeline->bind(commandBuffers[imageIndex]);
        lveModel->bind(commandBuffers[imageIndex]);
        lveModel->draw(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        VK_CHECK(vkEndCommandBuffer(commandBuffers[imageIndex]), "failed to record command buffer!");
    }

    void FirstApp::drawFrame() {
        uint32_t imageIndex;
        auto result = lveSwapChain->acquireNextImage(&imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        VK_CHECK_SWAPCHAIN(result, "failed to acquire swap chain image!");

        recordCommandBuffer(imageIndex);
        result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        } else if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
}  // namespace lve

// clang-fotmat off
// NOLINTEND(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix,
// *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init)
// clang-format on