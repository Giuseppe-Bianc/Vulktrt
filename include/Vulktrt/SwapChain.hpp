#pragma once

#include "Device.hpp"

namespace lve {
    class SwapChain {
    public:
        inline static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(Device &deviceRef, VkExtent2D windowExtent);
        SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);

        ~SwapChain();

        SwapChain(const SwapChain &) = delete;
        SwapChain &operator=(const SwapChain &) = delete;

        DISABLE_WARNINGS_PUSH(26446)
        VkFramebuffer getFrameBuffer(int index) noexcept { return swapChainFramebuffers[C_ST(index)]; }
        VkRenderPass getRenderPass() noexcept { return renderPass; }
        VkImageView getImageView(int index) noexcept { return swapChainImageViews[C_ST(index)]; }
        size_t imageCount() noexcept { return swapChainImages.size(); }
        VkFormat getSwapChainImageFormat() noexcept { return swapChainImageFormat; }
        VkExtent2D getSwapChainExtent() noexcept { return swapChainExtent; }
        uint32_t width() noexcept { return swapChainExtent.width; }
        uint32_t height() noexcept { return swapChainExtent.height; }

        float extentAspectRatio() noexcept { return C_F(swapChainExtent.width) / C_F(swapChainExtent.height); }
        DISABLE_WARNINGS_POP()

        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t *imageIndex) noexcept;
        VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

        bool compareSwapFormats(const SwapChain &swapChainin) const {
            return swapChainin.swapChainDepthFormat == swapChainDepthFormat &&
                   swapChainin.swapChainImageFormat == swapChainImageFormat;
        }

    private:
        void init();
        void createSwapChain();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat swapChainImageFormat;
        VkFormat swapChainDepthFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass renderPass;

        std::vector<VkImage> depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView> depthImageViews;
        std::vector<VkImage> swapChainImages;
        std::vector<VkImageView> swapChainImageViews;

        Device &device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;
        std::shared_ptr<SwapChain> oldSwapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    };

}  // namespace lve
