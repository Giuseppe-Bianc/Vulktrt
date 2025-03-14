// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-signed-bitwise, *-easily-swappable-parameters, *-use-anonymous-namespace, *-diagnostic-old-style-cast, *-pro-type-cstyle-cast, *-pro-type-member-init,*-member-init, *-pro-bounds-constant-array-index, *-qualified-auto, *-uppercase-literal-suffix)
// clang-format on
#include "Vulktrt/Device.hpp"
// #define INDEPTH

namespace lve {
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
        const std::string_view type = debugCallbackString(messageType);

        // Format and log the message
        const auto msg = FORMAT("{}Message ID: {}({}): {}", type, pCallbackData->pMessageIdName ? pCallbackData->pMessageIdName : "Unknown",
                                pCallbackData->messageIdNumber, pCallbackData->pMessage);

        printMessageWhitSeverity(msg, messageSeverity);

        logDebugValidationLayerInfo(pCallbackData, messageSeverity);

        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger) {
        // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if(func != nullptr) [[likely]] {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks *pAllocator) {
        // NOLINTNEXTLINE(*-pro-type-reinterpret-cast)
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if(func != nullptr) [[likely]] { func(instance, debugMessenger, pAllocator); }
    }

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

    std::string getVendorName(uint32_t vendorID) {
        static const std::unordered_map<uint32_t, std::string> vendorMap = {
            {0x1002, "Advanced Micro Devices, Inc. (AMD)"},
            {0x1010, "ImgTec"},
            {0x10DE, "NVIDIA Corporation"},
            {0x13B5, "ARM"},
            {0x5143, "Qualcomm"},
            {0x8086, "Intel Corporation"},
            {0x1A03, "ASPEED Technology"},
            {0x1D17, "Samsung Electronics Co Ltd"},
            {0x1E0F, "Huawei Technologies Co., Ltd."},
            {0x1B36, "Red Hat, Inc."},
            {0x1AF4, "Virtio"},
            {0x1C58, "JMicron Technology Corp."},
            {0x1106, "VIA Technologies, Inc."},
            {0x103C, "Hewlett-Packard Company"},
            {0x1022, "Advanced Micro Devices, Inc. (AMD)"},
            {0x102B, "Matrox Electronic Systems Ltd."},
            {0x1043, "ASUSTeK Computer Inc."},
            {0x1179, "Toshiba Corporation"},
            {0x11AB, "Marvell Technology Group Ltd."},
            {0x1237, "Intel Corporation (i440FX)"},
            {0x15B7, "SanDisk"},
            {0x168C, "Qualcomm Atheros"},
            {0x1912, "Renesas Electronics Corporation"},
            {0x1B4B, "Marvell Technology Group Ltd."},
            {0x1C5C, "Fresco Logic"},
            {0x1D6A, "Google, Inc."},
            {0x8087, "Intel Corporation"},
            {0x1057, "Motorola"},
            {0x1077, "QLogic Corp."},
            {0x1095, "Silicon Image, Inc."},
            {0x10EC, "Realtek Semiconductor Corp."},
            {0x11C1, "Lattice Semiconductor Corporation"},
            {0x14E4, "Broadcom Inc."},
            {0x15AD, "VMware, Inc."},
            {0x15BC, "Hitachi, Ltd."},
            {0x18D1, "Google Inc."},
            {0x1AE0, "Xilinx, Inc."},
            {0x1D0F, "Amazon.com, Inc."},
            {0x1C7C, "Tehuti Networks Ltd."},
            {0x16C3, "Cavium Inc."},
            {0x105A, "Promise Technology"},
            {0x12D8, "Pericom Semiconductor"},
            {0x1B21, "ASMedia Technology Inc."},
            {0x126F, "Silicon Motion, Inc."},
            {0x1137, "Cisco Systems Inc."},
            {0x1DB1, "Lite-On Technology Corporation"},
            {0x10B5, "PLX Technology, Inc."},
            {0x10B7, "3Com Corporation"},
            {0x15E8, "Solarflare Communications"},
            {0x1A3B, "Facebook, Inc."},
            {0x1CC4, "Innogrit, Inc."},
            {0x1C20, "Mellanox Technologies"},
            // Add more vendor IDs and their corresponding names here if needed
        };

        auto it = vendorMap.find(vendorID);
        if(it != vendorMap.end()) {
            return it->second;
        } else {
            return "Unknown Vendor";
        }
    }

    static const char *getDeviceType(VkPhysicalDeviceType input_value) {
        switch(input_value) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "OTHER";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "INTEGRATED_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "DISCRETE_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "VIRTUAL_GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        default:
            return "Unhandled VkPhysicalDeviceType";
        }
    }

    void printDeviceFeatures(VkPhysicalDevice device) {
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        VLINFO("=== Supported Features ===");
        VLINFO("Geometry Shader:       {}", C_BOOL(features.geometryShader));
        VLINFO("Tessellation Shader:   {}", C_BOOL(features.tessellationShader));
        VLINFO("Multi Viewport:        {}", C_BOOL(features.multiViewport));
        // Aggiungi altre features...
    }

    void printMemoryInfo(VkPhysicalDevice device) {
        VkPhysicalDeviceMemoryProperties mem_props;
        vkGetPhysicalDeviceMemoryProperties(device, &mem_props);

        const auto mhc = C_ST(mem_props.memoryHeapCount);
        VLINFO("=== Memory Heaps ({} total) ===", mhc);
        for(size_t i = 0; i < mhc; ++i) {
            const auto size_mb = C_ST(mem_props.memoryHeaps[i].size) / C_ST(1024 * 1024);
            const auto sis_device_local = (mem_props.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "[Device Local]" : "";
            VLINFO("Heap {:2}: {:>12} MB {}", i, size_mb, sis_device_local);
        }

        const auto mtc = C_ST(mem_props.memoryTypeCount);
        VLINFO("=== Memory Types ({} total) ===", mtc);
        for(size_t i = 0; i < mtc; ++i) {
            std::string flags;
            const auto &type = mem_props.memoryTypes[i];

            if(type.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) { flags += "DeviceLocal "; }
            if(type.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) { flags += "HostVisible "; }
            if(type.propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) { flags += "HostCoherent "; }

            VLINFO("Type {:2}: Heap {:2} | {}", i, type.heapIndex, flags);
        }
    }

    void printQueueFamilies(VkPhysicalDevice device) {
        uint32_t count{};
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
        std::vector<VkQueueFamilyProperties> queues(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queues.data());

        VLINFO("=== Queue Families ({} total) ===", count);
        for(uint32_t i = 0; i < queues.size(); ++i) {
            std::string capabilities;
            const auto &q = queues[i];

            if(q.queueFlags & VK_QUEUE_GRAPHICS_BIT) { capabilities += "Graphics "; }
            if(q.queueFlags & VK_QUEUE_COMPUTE_BIT) { capabilities += "Compute "; }
            if(q.queueFlags & VK_QUEUE_TRANSFER_BIT) { capabilities += "Transfer "; }
            if(q.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) { capabilities += "Sparse "; }

            VLINFO("Family {:2}: {:2} queues | {}", i, q.queueCount, capabilities);
        }
    }

    void printPhysicalDeviceProperties(const VkPhysicalDeviceProperties &properties) {
        VLINFO("Device Name: {}", properties.deviceName);
        VLINFO("API Version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion),
               VK_VERSION_PATCH(properties.apiVersion));
        VLINFO("Driver Version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion),
               VK_VERSION_PATCH(properties.driverVersion));
        VLINFO("Vendor ID: {}", getVendorName(properties.vendorID));
        VLINFO("Device ID: {0}(0x{0:04X})", properties.deviceID);
        VLINFO("Device Type: {}", getDeviceType(properties.deviceType));
        VLINFO("pipelineCacheUUID: {:02x}", FMT_JOIN(properties.pipelineCacheUUID, "-"));

        // Add more properties as needed
    }

    void printDeviceInfo(VkPhysicalDevice device, const VkPhysicalDeviceProperties &properties) {
        printPhysicalDeviceProperties(properties);
        printDeviceFeatures(device);
        printMemoryInfo(device);
        printQueueFamilies(device);
        // printDeviceExtensions(device);
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

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        if(enableValidationLayers) {
            createInfo.enabledLayerCount = C_UI32T(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            // NOLINTNEXTLINE(*-redundant-casting)
            createInfo.pNext = const_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debugCreateInfo);
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

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
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

        for(uint32_t queueFamily : uniqueQueueFamilies) {
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
        if(enableValidationLayers) {
            createInfo.enabledLayerCount = C_UI32T(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_), "failed to create logical device!");

        vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
        vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);
    }

    void Device::createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VK_CHECK(vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool), "failed to create command pool!");
    }

    void Device::createSurface() { window.createWindowSurface(instance, &surface_); }

    bool Device::isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if(extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
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

            if(std::ranges::any_of(availableLayers, [layerName](const auto &layerProperties) {
                   // NOLINTNEXTLINE(*-pro-bounds-array-to-pointer-decay,*-no-array-decay)
                   return std::strcmp(layerName, layerProperties.layerName) == 0;
               })) {
                layerFound = true;
            }

            if(!layerFound) { return false; }
        }

        return true;
    }

    std::vector<const char *> Device::getRequiredExtensions() const {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = nullptr;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if(enableValidationLayers) { extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }

        return extensions;
    }

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

        std::vector<std::string> availableExtensions;
        available.reserve(extensionCount);
        for(const auto &[extensionName, specVersion] : extensions) {
            availableExtensions.emplace_back(FORMAT("{} (v. {})", extensionName, specVersion));
            available.emplace(extensionName);
        }

        const auto requiredExtensions = getRequiredExtensions();
        for(const auto &required : requiredExtensions) {
            if(!available.contains(required)) [[unlikely]] { throw std::runtime_error("Missing required glfw extension"); }
        }
        VLINFO("available extensions:\n  {}", FMT_JOIN(availableExtensions, "\n  "));
        VLINFO("required extensions:\n  {}", FMT_JOIN(requiredExtensions, "\n  "));
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
        for(VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            // NOLINTBEGIN(*-branch-clone)
            if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
            // NOLINTEND(*-branch-clone)
        }
        throw std::runtime_error("failed to find supported format!");
    }

    uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags improperties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & improperties) == improperties) { return i; }
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

    VkCommandBuffer Device::beginSingleTimeCommands() {
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
        return commandBuffer;
    }

    void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue_);

        vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
    }

    void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void Device::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
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
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-signed-bitwise, *-easily-swappable-parameters, *-use-anonymous-namespace, *-diagnostic-old-style-cast, *-pro-type-cstyle-cast, *-pro-type-member-init,*-member-init, *-pro-bounds-constant-array-index, *-qualified-auto, *-uppercase-literal-suffix)
// clang-format on