// NOLINTBEGIN(*-include-cleaner, *-signed-bitwise, *-qualified-auto, *-pro-type-member-init, *-member-init, *-avoid-c-arrays)
#include "Vulktrt/SwapChain.hpp"

namespace lve {
    DISABLE_WARNINGS_PUSH(26446 26432 26485)
    // NOLINTBEGIN(*-diagnostic-unused-const-variable)
    inline static constexpr auto presentModeMailBox = VK_PRESENT_MODE_MAILBOX_KHR;
    inline static constexpr auto presentModeImmediate = VK_PRESENT_MODE_IMMEDIATE_KHR;
    // NOLINTEND(*-diagnostic-unused-const-variable)

    SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent) : device{deviceRef}, windowExtent{extent} { init(); }

    SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous)
      : device{deviceRef}, windowExtent{extent}, oldSwapChain{std::move(previous)} {
        init();
        oldSwapChain = nullptr;
    }

    void SwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }

    SwapChain::~SwapChain() {
        const auto dev = device.device();
        for (const auto &imageView : swapChainImageViews) {
            vkDestroyImageView(dev, imageView, nullptr);
        }
        // vkDestroySwapchainKHR should only be called if swapChain is valid.
        if (swapChain != VK_NULL_HANDLE) {
            vkDestroySwapchainKHR(dev, swapChain, nullptr);
            swapChain = VK_NULL_HANDLE;
        }
        for (size_t i = 0; i < depthImages.size(); i++) {
            vkDestroyImageView(dev, depthImageViews[i], nullptr);
            vkDestroyImage(dev, depthImages[i], nullptr);
            vkFreeMemory(dev, depthImageMemorys[i], nullptr);
        }
        for (const auto &framebuffer : swapChainFramebuffers) {
            vkDestroyFramebuffer(dev, framebuffer, nullptr);
        }
        vkDestroyRenderPass(dev, renderPass, nullptr);
        // Cleanup synchronization objects
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(dev, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(dev, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(dev, inFlightFences[i], nullptr);
        }
    }

    VkResult SwapChain::acquireNextImage(uint32_t *imageIndex) noexcept {
        vkWaitForFences(device.device(), 1, &inFlightFences[currentFrame], VK_TRUE, uint64Max);
        return vkAcquireNextImageKHR(device.device(), swapChain, uint64Max,
                                     imageAvailableSemaphores[currentFrame],
                                     VK_NULL_HANDLE, imageIndex);
    }

    DISABLE_WARNINGS_PUSH(26429 26461)
    VkResult SwapChain::submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex) {
        if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device.device(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        const VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        const VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(device.device(), 1, &inFlightFences[currentFrame]);
        VK_CHECK(vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]),
                 "failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        const VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = imageIndex;

        const auto result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }
    // NOLINTEND(*-non-const-parameter, *-pro-bounds-array-to-pointer-decay,*-no-array-decay)
    DISABLE_WARNINGS_POP()

    void SwapChain::createSwapChain() {
        const auto swapChainSupport = device.getSwapChainSupport();
        const VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        const VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        const VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = device.surface();

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const auto indices = device.findPhysicalQueueFamilies();
        std::array<std::uint32_t, 2> queueFamilyIndices = { indices.graphicsFamily, indices.presentFamily };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount =C_UI32T(queueFamilyIndices.size());
            createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;      // Optional
            createInfo.pQueueFamilyIndices = nullptr;  // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = (oldSwapChain == nullptr) ? VK_NULL_HANDLE : oldSwapChain->swapChain;

        VK_CHECK(vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain), "failed to create swap chain!");
        device.setObjectName(VK_OBJECT_TYPE_SWAPCHAIN_KHR, BC_UI64T(swapChain), "sw swapchain");

        // we only specified a minimum number of images in the swap chain, so the implementation is
        // allowed to create a swap chain with more. That's why we'll first query the final number of
        // images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
        // retrieve the handles.
        vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, swapChainImages.data());
        device.setObjectNames(VK_OBJECT_TYPE_IMAGE, "SwapChain Image", swapChainImages);

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void SwapChain::createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());
        const auto dev = device.device();
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = swapChainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = swapChainImageFormat;
            viewInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                    VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            VK_CHECK(vkCreateImageView(dev, &viewInfo, nullptr, &swapChainImageViews[i]), "failed to create texture image view!");
        }
        device.setObjectNames(VK_OBJECT_TYPE_IMAGE_VIEW, "sw Image view", swapChainImageViews);
    }

    void SwapChain::createRenderPass() {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = getSwapChainImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.dstSubpass = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = C_UI32T(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        VK_CHECK(vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &renderPass), "failed to create render pass!");
        device.setObjectName(VK_OBJECT_TYPE_RENDER_PASS, BC_UI64T(renderPass), "Main Render Pass");
    }

    void SwapChain::createFramebuffers() {
        swapChainFramebuffers.resize(imageCount());
        const auto dev = device.device();
        for (size_t i = 0; i < imageCount(); i++) {
            std::array<VkImageView, 2> attachments = { swapChainImageViews[i], depthImageViews[i] };
            auto [width, height] = getSwapChainExtent();
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = C_UI32T(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;
            VK_CHECK(vkCreateFramebuffer(dev, &framebufferInfo, nullptr, &swapChainFramebuffers[i]),
                     "failed to create framebuffer!");
        }
        device.setObjectNames(VK_OBJECT_TYPE_FRAMEBUFFER, "Framebuffer", swapChainFramebuffers);
    }

    void SwapChain::createDepthResources() {
        const VkFormat depthFormat = findDepthFormat();
        swapChainDepthFormat = depthFormat;
        const VkExtent2D iswapChainExtent = getSwapChainExtent();

        const size_t count = imageCount();
        depthImages.resize(count);
        depthImageMemorys.resize(count);
        depthImageViews.resize(count);

        for (size_t i = 0; i < count; i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = iswapChainExtent.width;
            imageInfo.extent.height = iswapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = depthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            VK_CHECK(vkCreateImageView(device.device(), &viewInfo, nullptr, &depthImageViews[i]), "failed to create texture image view!");
        }
        device.setObjectNames(VK_OBJECT_TYPE_IMAGE, "Depth Image", depthImages);
        device.setObjectNames(VK_OBJECT_TYPE_IMAGE_VIEW, "Depth Image View", depthImageViews);
        device.setObjectNames(VK_OBJECT_TYPE_DEVICE_MEMORY, "Depth Image Memory", depthImageMemorys);
    }

    void SwapChain::createSyncObjects() {
        const size_t count = imageCount();
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(count, VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        const auto dev = device.device();
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VK_CHECK_SYNC_OBJECTS(vkCreateSemaphore(dev, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]),
                                  vkCreateSemaphore(dev, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]),
                                  vkCreateFence(dev, &fenceInfo, nullptr, &inFlightFences[i]),
                                  "failed to create synchronization objects for a frame!");
        }
        device.setObjectNames(VK_OBJECT_TYPE_SEMAPHORE, "Image Available Semaphore", imageAvailableSemaphores);
        device.setObjectNames(VK_OBJECT_TYPE_SEMAPHORE, "Render Finished Semaphore", renderFinishedSemaphores);
        device.setObjectNames(VK_OBJECT_TYPE_FENCE, "In Flight Fence", inFlightFences);
    }

    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        auto isDesiredFormat = [](const VkSurfaceFormatKHR &format) {
            return format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        };

        if(auto it = std::ranges::find_if(availableFormats, isDesiredFormat); it != availableFormats.end()) { return *it; }

        return availableFormats[0];
    }

    VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        // if(std::ranges::find(availablePresentModes, presentModeMailBox) != availablePresentModes.end()) {
        //     VLINFO("Present mode: Mailbox");
        //     return presentModeMailBox;
        // }

        if(std::ranges::find(availablePresentModes, presentModeImmediate) != availablePresentModes.end()) {
            VLINFO("Present mode: Immediate");
            return presentModeImmediate;
        }
        VLINFO("Present mode: V-Sync");
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if(capabilities.currentExtent.width != ui32tmax) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = windowExtent;
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkFormat SwapChain::findDepthFormat() {
        return device.findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                          VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }
    DISABLE_WARNINGS_POP()
}  // namespace lve

// NOLINTEND(*-include-cleaner, *-signed-bitwise, *-qualified-auto, *-pro-type-member-init, *-member-init, *-avoid-c-arrays)
