/*
 * Created by gbian on 18/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto)
// clang-format on
#include "Vulktrt/Renderer.hpp"

namespace lve {
    Renderer::Renderer(Window &window, Device &device) noexcept : lveWindow{window}, lveDevice{device} {
        recreateSwapChain();
        createCommandBuffers();
    }

    Renderer::~Renderer() { freeCommandBuffers(); }

    void Renderer::recreateSwapChain() {
        auto extent = lveWindow.getExtent();
        while(extent.width == 0 || extent.height == 0) {
            extent = lveWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(lveDevice.device());

        if(lveSwapChain == nullptr) {
            lveSwapChain = std::make_unique<SwapChain>(lveDevice, extent);
        } else {
            // Using move semantics to transfer ownership to a shared pointer
            std::shared_ptr<SwapChain> oldSwapChain = std::move(lveSwapChain);
            lveSwapChain = std::make_unique<SwapChain>(lveDevice, extent, oldSwapChain);

            if(!oldSwapChain->compareSwapFormats(*lveSwapChain)) {
                throw std::runtime_error("Swap chain image (or depth) format has changed!");
            }
        }
    }

    void Renderer::createCommandBuffers() {
        commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = C_UI32T(commandBuffers.size());

        VK_CHECK(vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()), "failed to allocate command buffers!");
        // lveDevice.setObjectsName(VK_OBJECT_TYPE_COMMAND_BUFFER, "Main Command Buffers", commandBuffers);
        lveDevice.setObjectNames(VK_OBJECT_TYPE_COMMAND_BUFFER, "Main Command Buffers", commandBuffers);
    }

    void Renderer::freeCommandBuffers() {
        vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), C_UI32T(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer Renderer::beginFrame() {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        VK_CHECK_SWAPCHAIN(result, "failed to acquire swap chain image!");

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo), "failed to begin recording command buffer!");
        lveDevice.setObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, BC_UI64T(commandBuffer), "Frame Command Buffer");

        // Begin command buffer label for debugging.
        lveDevice.cmdBeginLabel(commandBuffer, "Frame Command Buffer Begin", {0.0f, 1.0f, 0.0f, 1.0f});

        return commandBuffer;
    }

    void Renderer::endFrame() {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();

        // End command buffer label
        lveDevice.cmdEndLabel(commandBuffer);

        VK_CHECK(vkEndCommandBuffer(commandBuffer), "failed to record command buffer!");

        auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()) {
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if(result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();

        // Cache the framebuffer and extent to avoid redundant calls.
        auto currentFramebuffer = lveSwapChain->getFrameBuffer(NC_I(currentImageIndex));
        auto swapChainExtent = lveSwapChain->getSwapChainExtent();
        renderPassInfo.framebuffer = currentFramebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = C_UI32T(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        lveDevice.cmdInsertLabel(commandBuffer, "Render Pass Begin", {0.0f, 0.0f, 1.0f, 1.0f});
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Set viewport and scissor using the cached extent.
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = C_F(swapChainExtent.width);
        viewport.height = C_F(swapChainExtent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChainExtent};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
        lveDevice.cmdInsertLabel(commandBuffer, "Render Pass End", {1.0f, 0.0f, 0.0f, 1.0f});
    }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto)
// clang-format on
