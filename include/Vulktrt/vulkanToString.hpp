/*
* Created by gbian on 15/03/2025.
* Copyright (c) 2025 All rights reserved.
*/
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "headers.hpp"

[[nodiscard]] static constexpr std::string_view debugCallbackString(VkDebugUtilsMessageTypeFlagsEXT messageType) noexcept {
    switch(messageType) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        return "[General] ";
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        return "[Validation] ";
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        return "[Performance] ";
    default:
        return "";
    }
}

static inline const char *VkMemoryPropertyFlagBitsString(VkMemoryPropertyFlagBits input_value) {
    switch(input_value) {
    case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
        return "DEVICE_LOCAL";
    case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
        return "HOST_VISIBLE";
    case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
        return "HOST_COHERENT";
    case VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
        return "HOST_CACHED";
    case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
        return "LAZILY_ALLOCATED";
    case VK_MEMORY_PROPERTY_PROTECTED_BIT:
        return "PROTECTED";
    case VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD:
        return "DEVICE_COHERENT";
    case VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD:
        return "DEVICE_UNCACHED";
    case VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV:
        return "RDMA_CAPABLE";
    default:
        return "Unhandled VkMemoryPropertyFlagBits";
    }
}

static inline const char *VkQueueFlagBitsString(VkQueueFlagBits input_value) {
    switch(input_value) {
    case VK_QUEUE_GRAPHICS_BIT:
        return "GRAPHICS";
    case VK_QUEUE_COMPUTE_BIT:
        return "COMPUTE";
    case VK_QUEUE_TRANSFER_BIT:
        return "TRANSFER";
    case VK_QUEUE_SPARSE_BINDING_BIT:
        return "SPARSE_BINDING";
    case VK_QUEUE_PROTECTED_BIT:
        return "PROTECTED";
    case VK_QUEUE_VIDEO_DECODE_BIT_KHR:
        return "VIDEO_DECODE";
    case VK_QUEUE_VIDEO_ENCODE_BIT_KHR:
        return "VIDEO_ENCODE";
    case VK_QUEUE_OPTICAL_FLOW_BIT_NV:
        return "OPTICAL_FLOW";
    default:
        return "Unhandled VkQueueFlagBits";
    }
}

static inline std::string VkMemoryPropertyFlagsString(VkMemoryPropertyFlags input_value) {
    std::string ret;
    int index = 0;
    while(input_value) {
        if(input_value & 1) {
            if(!ret.empty()) ret.append(" |");
            ret.append(VkMemoryPropertyFlagBitsString(static_cast<VkMemoryPropertyFlagBits>(1U << index)));
        }
        ++index;
        input_value >>= 1;
    }
    // if(ret.empty()) ret.append("VkMemoryPropertyFlags(0)");
    return ret;
}

static inline std::string VkQueueFlagsString(VkQueueFlags input_value) {
    std::string ret;
    int index = 0;
    while(input_value) {
        if(input_value & 1) {
            if(!ret.empty()) ret.append(" |");
            ret.append(VkQueueFlagBitsString(static_cast<VkQueueFlagBits>(1U << index)));
        }
        ++index;
        input_value >>= 1;
    }
    // if(ret.empty()) ret.append("VkQueueFlags(0)");
    return ret;
}


// NOLINTEND(*-include-cleaner)