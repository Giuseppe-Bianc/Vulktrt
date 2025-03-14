/*
 * Created by gbian on 10/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "vulkanCheck.hpp"
#include <vulkan/vk_enum_string_helper.h>
#define USE_FALLTHROUGH

DISABLE_WARNINGS_PUSH(26429 26481)

// const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
inline static void printMessageWhitSeverity(const std::string &msg, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity) {
    switch(messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        VLTRACE(msg);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        VLINFO(msg);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        VLWARN(msg);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        VLERROR(msg);
        break;
    default:
        VLDEBUG(msg);
        break;
    }
}

inline static void logQueueLabel(const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                 const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity) {
    for(uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
        std::string_view labelName = pCallbackData->pQueueLabels[i].pLabelName ? pCallbackData->pQueueLabels[i].pLabelName : "Unknown";
        const auto msg = FORMAT("Queue Label [{}]: {}", i, labelName);

        printMessageWhitSeverity(msg, messageSeverity);
    }
}

inline static void logCmdBuffers(const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                 const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity) {
    for(uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; ++i) {
        std::string_view labelName = pCallbackData->pCmdBufLabels[i].pLabelName ? pCallbackData->pCmdBufLabels[i].pLabelName : "Unknown";
        const auto msg = FORMAT("Command Buffer Label[{}] - {} {{ {}, {}, {}, {} }}", i, labelName,
                                pCallbackData->pCmdBufLabels[i].color[0], pCallbackData->pCmdBufLabels[i].color[1],
                                pCallbackData->pCmdBufLabels[i].color[2], pCallbackData->pCmdBufLabels[i].color[3]);

        printMessageWhitSeverity(msg, messageSeverity);
    }
}

inline static void logObjects(const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                              const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity) {
    for(uint32_t i = 0; i < pCallbackData->objectCount; ++i) {
        std::string_view objectName = pCallbackData->pObjects[i].pObjectName ? pCallbackData->pObjects[i].pObjectName : "Unknown";
        const auto objectType = pCallbackData->pObjects[i].objectType;
        const auto objhandle = pCallbackData->pObjects[i].objectHandle;
        const auto objtypestring = string_VkObjectType(objectType);
        std::string msg;
        // NOLINTBEGIN(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
        switch(objectType) {
        case VK_OBJECT_TYPE_INSTANCE:
        case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
        case VK_OBJECT_TYPE_DEVICE:
        case VK_OBJECT_TYPE_COMMAND_BUFFER:
#ifdef USE_FALLTHROUGH
            [[fallthrough]];
#endif  // USE_FALLTHROUGH
        case VK_OBJECT_TYPE_QUEUE:
            msg = FORMAT("Object [{}]: Type: {} (Handle: {}) Name: {}", i, objtypestring,
                         reinterpret_cast<void *>(static_cast<uintptr_t>(objhandle)), objectName);
            break;
        default:
            msg = FORMAT("Object [{}]: Type: {} (Handle: 0x{:X}) Name: {}", i, objtypestring, objhandle, objectName);
            break;
        }
        // NOLINTEND(*-pro-type-reinterpret-cast, *-no-int-to-ptr)
        printMessageWhitSeverity(msg, messageSeverity);
    }
}

inline void logDebugValidationLayerInfo(const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                        const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity) {
    // Log queue labels if available
    logQueueLabel(pCallbackData, messageSeverity);

    // Log command buffer labels if available
    logCmdBuffers(pCallbackData, messageSeverity);

    // Log objects if available
    logObjects(pCallbackData, messageSeverity);
}

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

DISABLE_WARNINGS_POP()

// NOLINTEND(*-include-cleaner)
