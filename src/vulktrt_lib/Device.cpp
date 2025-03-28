// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-signed-bitwise, *-easily-swappable-parameters, *-use-anonymous-namespace, *-diagnostic-old-style-cast, *-pro-type-cstyle-cast, *-pro-type-member-init,*-member-init, *-pro-bounds-constant-array-index, *-qualified-auto, *-uppercase-literal-suffix)
// clang-format on
#include "Vulktrt/Device.hpp"
// #define INDEPTH
#define USE_ALGORITHM

namespace lve {
    DISABLE_WARNINGS_PUSH(26446 26482 26485)
    static inline constexpr float queuePriority = 1.0f;
    // local callback functions
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        [[maybe_unused]] void *pUserData) {
#ifdef INDEPTH
        vnd::AutoTimer t{"Debug Callback", vnd::Timer::Big};
#endif
        // Determine the message type
        const std::string type = VkDebugUtilsMessageTypeFlagsEXTString(messageType);

        // Format and log the message
        const auto msg = FORMAT("{}Message ID: {}({}): {}", type, pCallbackData->pMessageIdName ? pCallbackData->pMessageIdName : "Unknown",
                                pCallbackData->messageIdNumber, pCallbackData->pMessage);

        printMessageWhitSeverity(msg, messageSeverity);

        logDebugValidationLayerInfo(pCallbackData, messageSeverity);

        return VK_FALSE;
    }

    // NOLINTBEGIN(*-use-internal-linkage)
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instancein, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) noexcept {
        // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
        auto func = std::bit_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instancein, "vkCreateDebugUtilsMessengerEXT"));
        // clang-format off
        if(func != nullptr) [[likely]] {
            return func(instancein, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        // clang-format on
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instancein, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator) noexcept {
        // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
        auto func = std::bit_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instancein, "vkDestroyDebugUtilsMessengerEXT"));
        if(func != nullptr) [[likely]] { func(instancein, debugMessenger, pAllocator); }
    }
    // NOLINTEND(*-use-internal-linkage)

    // class member functions
    Device::Device(Window &windowe) : window{windowe} {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    }

    Device::~Device() {
        vkDestroyCommandPool(device_, commandPool, nullptr);
        vkDestroyDevice(device_, nullptr);

        if(enableValidationLayers) { DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr); }

        vkDestroySurfaceKHR(instance, surface_, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    // NOLINTBEGIN(*-make-member-function-const, *-pro-bounds-array-to-pointer-decay, *-no-array-decay)
    // Wrapper per vkCmdBeginDebugUtilsLabelEXT
    void Device::pcmdBeginLabel(VkInstance instancein, VkCommandBuffer commandBuffer, const char *labelName,
                                const std::vector<float> &color) noexcept {
        if(!enableValidationLayers) { return; }
        auto func = std::bit_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instancein, "vkCmdBeginDebugUtilsLabelEXT"));
        if(func != nullptr) {
            VkDebugUtilsLabelEXT label{};
            label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            label.pLabelName = labelName;
            memcpy(label.color, color.data(), sizeof(float) * 4);
            func(commandBuffer, &label);
        }
    }

    // Wrapper per vkCmdInsertDebugUtilsLabelEXT
    void Device::pcmdInsertLabel(VkInstance instancein, VkCommandBuffer commandBuffer, const char *labelName,
                                 const std::vector<float> &color) noexcept {
        if(!enableValidationLayers) { return; }
        auto func = std::bit_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instancein, "vkCmdInsertDebugUtilsLabelEXT"));
        if(func != nullptr) {
            VkDebugUtilsLabelEXT label{};
            label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            label.pLabelName = labelName;
            memcpy(label.color, color.data(), sizeof(float) * 4);
            func(commandBuffer, &label);
        }
    }

    // Wrapper per vkCmdEndDebugUtilsLabelEXT
    void Device::pcmdEndLabel(VkInstance instancein, VkCommandBuffer commandBuffer) noexcept {
        if(!enableValidationLayers) { return; }
        auto func = std::bit_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instancein, "vkCmdEndDebugUtilsLabelEXT"));
        if(func != nullptr) { func(commandBuffer); }
    }

    // Wrapper per vkQueueBeginDebugUtilsLabelEXT
    void Device::pqueueBeginLabel(VkInstance instancein, VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept {
        if(!enableValidationLayers) { return; }
        auto func = std::bit_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instancein, "vkQueueBeginDebugUtilsLabelEXT"));
        if(func != nullptr) {
            VkDebugUtilsLabelEXT label{};
            label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            label.pLabelName = labelName;
            memcpy(label.color, color.data(), sizeof(float) * 4);
            func(queue, &label);
        }
    }

    // Wrapper per vkQueueInsertDebugUtilsLabelEXT
    void Device::pqueueInsertLabel(VkInstance instancein, VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept {
        if(!enableValidationLayers) { return; }
        auto func = std::bit_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(
            vkGetInstanceProcAddr(instancein, "vkQueueInsertDebugUtilsLabelEXT"));
        if(func != nullptr) {
            VkDebugUtilsLabelEXT label{};
            label.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
            label.pLabelName = labelName;
            memcpy(label.color, color.data(), sizeof(float) * 4);
            func(queue, &label);
        }
    }

    // Wrapper per vkQueueEndDebugUtilsLabelEXT
    void Device::pqueueEndLabel(VkInstance instancein, VkQueue queue) noexcept {
        if(!enableValidationLayers) { return; }
        auto func = std::bit_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instancein, "vkQueueEndDebugUtilsLabelEXT"));
        if(func != nullptr) { func(queue); }
    }

    // Wrapper per vkSetDebugUtilsObjectNameEXT
    void Device::psetObjectName(VkInstance instancein, VkDevice device, VkObjectType objectType, uint64_t objectHandle,
                                const char *objectName) noexcept {
        if(!enableValidationLayers) { return; }
        auto func = std::bit_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instancein, "vkSetDebugUtilsObjectNameEXT"));
        if(func != nullptr) {
            VkDebugUtilsObjectNameInfoEXT nameInfo{};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = objectType;
            nameInfo.objectHandle = objectHandle;
            nameInfo.pObjectName = objectName;
            func(device, &nameInfo);
        }
    }
    // NOLINTEND(*-make-member-function-const, *-pro-bounds-array-to-pointer-decay, *-no-array-decay)

    void Device::cmdBeginLabel(VkCommandBuffer commandBuffer, const char *labelName, const std::vector<float> &color) noexcept {
        pcmdBeginLabel(instance, commandBuffer, labelName, color);
    }
    void Device::cmdEndLabel(VkCommandBuffer commandBuffer) noexcept { pcmdEndLabel(instance, commandBuffer); }
    void Device::cmdInsertLabel(VkCommandBuffer commandBuffer, const char *labelName, const std::vector<float> &color) noexcept {
        pcmdInsertLabel(instance, commandBuffer, labelName, color);
    }
    void Device::queueBeginLabel(VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept {
        pqueueBeginLabel(instance, queue, labelName, color);
    }

    void Device::queueEndLabel(VkQueue queue) noexcept { pqueueEndLabel(instance, queue); }
    void Device::queueInsertLabel(VkQueue queue, const char *labelName, const std::vector<float> &color) noexcept {
        pqueueInsertLabel(instance, queue, labelName, color);
    }

    void Device::setObjectName(VkObjectType objectType, uint64_t objectHandle, const char *objectName) noexcept {
        psetObjectName(instance, device_, objectType, objectHandle, objectName);
    }

    void Device::createInstance() {
        if(enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "LittleVulkanEngine App";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = C_UI32T(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
#ifdef NDEBUG
        if(enableValidationLayers) [[unlikely]] {
            createInfo.enabledLayerCount = C_UI32T(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        } else [[likely]] {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }
#else
        if(enableValidationLayers) [[likely]] {
            createInfo.enabledLayerCount = C_UI32T(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = &debugCreateInfo;
        } else [[unlikely]] {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }
#endif

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance), "failed to create instance!");

        hasGflwRequiredInstanceExtensions();
    }

    void Device::pickPhysicalDevice() {
#ifdef INDEPTH
        vnd::AutoTimer t{"pickPhysicalDevice", vnd::Timer::Big};
#endif
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if(deviceCount == 0) { throw std::runtime_error("failed to find GPUs with Vulkan support!"); }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for(const auto &device : devices) {
            if(isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if(physicalDevice == VK_NULL_HANDLE) { throw std::runtime_error("failed to find a suitable GPU!"); }

        vkGetPhysicalDeviceProperties(physicalDevice, &properties);
        VLINFO("Dev count: {}", deviceCount);
        printDeviceInfo(physicalDevice, properties);
    }

    void Device::createLogicalDevice() {
        const QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

        for(const uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = C_UI32T(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = C_UI32T(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        // might not really be necessary anymore because device specific validation layers
        // have been deprecated
#ifdef NDEBUG
        if(enableValidationLayers) [[unlikely]] {
            createInfo.enabledLayerCount = NC_UI32T(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else [[likely]] {
            createInfo.enabledLayerCount = 0;
        }
#else
        if(enableValidationLayers) [[likely]] {
            createInfo.enabledLayerCount = NC_UI32T(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else [[unlikely]] {
            createInfo.enabledLayerCount = 0;
        }
#endif

        VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_), "failed to create logical device!");
        auto device = device_;
        auto dinstance = instance;
        psetObjectName(instance, device_, VK_OBJECT_TYPE_INSTANCE, BC_UI64T(dinstance), "Main Instance");
        psetObjectName(instance, device_, VK_OBJECT_TYPE_DEVICE, BC_UI64T(device), "Main Device");
        psetObjectName(instance, device_, VK_OBJECT_TYPE_PHYSICAL_DEVICE, BC_UI64T(physicalDevice), "Main Physical Device");

        vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
        vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);
        psetObjectName(instance, device_, VK_OBJECT_TYPE_QUEUE, BC_UI64T(graphicsQueue_), "Graphics Queue");
        psetObjectName(instance, device_, VK_OBJECT_TYPE_QUEUE, BC_UI64T(presentQueue_), "Present Queue");
    }

    void Device::createCommandPool() {
        const QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK(vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool), "failed to create command pool!");

        psetObjectName(instance, device_, VK_OBJECT_TYPE_COMMAND_POOL, BC_UI64T(commandPool), "Main Command Pool");
    }

    void Device::createSurface() { window.createWindowSurface(instance, &surface_); }

    bool Device::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        const bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if(extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;  // Optional
    }

    void Device::setupDebugMessenger() {
        if(!enableValidationLayers) { return; }
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);
        VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger), "failed to set up debug messenger!");
    }

    bool Device::checkValidationLayerSupport() {
#ifdef INDEPTH
        vnd::AutoTimer t{"checkValidationLayerSupport", vnd::Timer::Big};
#endif
        uint32_t layerCount{};
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for(const char *layerName : validationLayers) {
            bool layerFound = false;

            // clang-format off
            if(std::ranges::any_of(availableLayers, [layerName](const auto &layerProperties) {
                   // NOLINTNEXTLINE(*-pro-bounds-array-to-pointer-decay,*-no-array-decay)
                   return std::strcmp(layerName, layerProperties.layerName) == 0;
               })) {
                layerFound = true;
            }
            // clang-format on

            if(!layerFound) { return false; }
        }

        return true;
    }

    DISABLE_WARNINGS_PUSH(26481)

    std::vector<const char *> Device::getRequiredExtensions() const {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = nullptr;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if(enableValidationLayers) { extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }

        return extensions;
    }

    DISABLE_WARNINGS_POP()

    void Device::hasGflwRequiredInstanceExtensions() {
#ifdef INDEPTH
        vnd::AutoTimer t{"hasGflwRequiredInstanceExtensions", vnd::Timer::Big};
#endif
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        std::unordered_set<std::string_view> available;
        available.reserve(extensionCount);  // Riserviamo spazio per migliorare le prestazioni

#if defined(_DEBUG) || defined(DEBUG)
        std::vector<std::string> availableExtensions;
#endif
        available.reserve(extensionCount);
        for(const auto &[extensionName, specVersion] : extensions) {
#if defined(_DEBUG) || defined(DEBUG)
            availableExtensions.emplace_back(FORMAT("{} (v. {})", extensionName, specVersion));
#endif
            available.emplace(extensionName);
        }

        const auto requiredExtensions = getRequiredExtensions();
        if(!std::ranges::all_of(requiredExtensions, [&](const auto &required) { return available.contains(required); })) [[unlikely]] {
            throw std::runtime_error("Missing required GLFW extension");
        }
#ifdef NDEBUG
        VLINFO("available extensions count: {}", available.size());
        VLINFO("required extensions count: {}", requiredExtensions.size());
#else
        VLINFO("available extensions:\n  {}", FMT_JOIN(availableExtensions, "\n  "));
        VLINFO("required extensions:\n  {}", FMT_JOIN(requiredExtensions, "\n  "));
#endif
    }

    bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount{};
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for(const auto &extension : availableExtensions) {
            // NOLINTNEXTLINE(*-pro-bounds-array-to-pointer-decay, *-no-array-decay)
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for(const auto &[i, queueFamily] : queueFamilies | std::views::enumerate) {
            if(queueFamily.queueCount > 0 && C_BOOL(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                indices.graphicsFamily = C_UI32T(i);
                indices.graphicsFamilyHasValue = true;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, C_UI32T(i), surface_, &presentSupport);
            if(queueFamily.queueCount > 0 && C_BOOL(presentSupport)) {
                indices.presentFamily = C_UI32T(i);
                indices.presentFamilyHasValue = true;
            }
            if(indices.isComplete()) { break; }
        }

        return indices;
    }

    SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

        uint32_t formatCount{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

        if(formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

        if(presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    VkFormat Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for(const VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            // NOLINTBEGIN(*-branch-clone)
            // clang-format off
            if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
            // clang-format on
            // NOLINTEND(*-branch-clone)
        }
        throw std::runtime_error("failed to find supported format!");
    }

    uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags improperties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        const std::bitset<32> typeBits(typeFilter);
        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if(typeBits.test(i) && (memProperties.memoryTypes[i].propertyFlags & improperties) == improperties) { return i; }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags improperties, VkBuffer &buffer,
                              VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK(vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer), "failed to create vertex buffer!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, improperties);

        VK_CHECK(vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory), "failed to allocate vertex buffer memory!");

        vkBindBufferMemory(device_, buffer, bufferMemory, 0);
    }

    VkCommandBuffer Device::beginSingleTimeCommands() noexcept {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer{};
        vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        pcmdBeginLabel(instance, commandBuffer, "Begin Single Time Commands", {0.0f, 1.0f, 0.0f, 1.0f});

        return commandBuffer;
    }

    void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) noexcept {
        // Chiude la label inserita
        pcmdEndLabel(instance, commandBuffer);
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        pqueueBeginLabel(instance, graphicsQueue_, "Submit Single Time Command", {1.0f, 1.0f, 1.0f, 1.0f});
        vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue_);
        pqueueEndLabel(instance, graphicsQueue_);

        vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
    }

    void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) noexcept {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void Device::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) noexcept {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = layerCount;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
        endSingleTimeCommands(commandBuffer);
    }

    void Device::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags improperties, VkImage &image,
                                     VkDeviceMemory &imageMemory) {
        VK_CHECK(vkCreateImage(device_, &imageInfo, nullptr, &image), "failed to create image!");

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device_, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, improperties);

        VK_CHECK(vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory), "failed to allocate image memory!");

        if(vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS) { throw std::runtime_error("failed to bind image memory!"); }
    }

    DISABLE_WARNINGS_POP()
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-signed-bitwise, *-easily-swappable-parameters, *-use-anonymous-namespace, *-diagnostic-old-style-cast, *-pro-type-cstyle-cast, *-pro-type-member-init,*-member-init, *-pro-bounds-constant-array-index, *-qualified-auto, *-uppercase-literal-suffix)
// clang-format on
