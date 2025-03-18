// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "DeviceInfo.hpp"
#include "VulkanLogInfoCallback.hpp"
#include "Window.hpp"

namespace lve {
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        uint32_t graphicsFamily{};
        uint32_t presentFamily{};
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() noexcept { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

    class Device {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        explicit Device(Window &windowe);
        ~Device();

        // Not copyable or movable
        Device(const Device &) = delete;
        void operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

        VkCommandPool getCommandPool() noexcept { return commandPool; }
        VkDevice device() noexcept { return device_; }
        VkSurfaceKHR surface() noexcept { return surface_; }
        VkQueue graphicsQueue() noexcept { return graphicsQueue_; }
        VkQueue presentQueue() noexcept { return presentQueue_; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags improperties);
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
        VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags improperties, VkBuffer &buffer,
                          VkDeviceMemory &bufferMemory);
        VkCommandBuffer beginSingleTimeCommands() noexcept;
        void endSingleTimeCommands(VkCommandBuffer commandBuffer) noexcept;
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) noexcept;
        void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) noexcept;

        void createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags improperties, VkImage &image,
                                 VkDeviceMemory &imageMemory);

        void cmdBeginLabel(VkCommandBuffer commandBuffer, const char *labelName, const std::vector<float> &color) noexcept;
        void cmdInsertLabel(VkCommandBuffer commandBuffer, const char *labelName, const std::vector<float> &color) noexcept;
        void cmdEndLabel(VkCommandBuffer commandBuffer) noexcept;
        void queueBeginLabel(VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept;
        void queueInsertLabel(VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept;
        void queueEndLabel(VkQueue queue) noexcept;
        void setObjectName(VkObjectType objectType, uint64_t objectHandle, const char *objectName) noexcept;
        template <typename T> void setObjectNames(VkObjectType objectType, const char* objectName, const std::vector<T>& objectHandles) noexcept {
            for (auto [index, objectHandle] : objectHandles | std::views::enumerate) {
                psetObjectName(instance, device_, objectType,  BC_UI64T(objectHandle), FORMAT("{} {}", objectName, index).c_str());
            }
        }

        VkPhysicalDeviceProperties properties;

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();
        void pcmdBeginLabel(VkInstance instancein, VkCommandBuffer commandBuffer, const char *labelName,
                            const std::vector<float> &color) noexcept;
        void pcmdInsertLabel(VkInstance instancein, VkCommandBuffer commandBuffer, const char *labelName,
                             const std::vector<float> &color) noexcept;
        void pcmdEndLabel(VkInstance instancein, VkCommandBuffer commandBuffer) noexcept;
        void pqueueBeginLabel(VkInstance instancein, VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept;
        void pqueueInsertLabel(VkInstance instancein, VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept;
        void pqueueEndLabel(VkInstance instancein, VkQueue queue) noexcept;
        void psetObjectName(VkInstance instancein, VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                            const char *objectName) noexcept;

        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char *> getRequiredExtensions() const;
        bool checkValidationLayerSupport();
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept;
        void hasGflwRequiredInstanceExtensions();
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        Window &window;
        VkCommandPool commandPool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphicsQueue_;
        VkQueue presentQueue_;

        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
}  // namespace lve

// NOLINTEND(*-include-cleaner)
