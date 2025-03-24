/*
 * Created by gbian on 22/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner,*-easily-swappable-parameters, *-prefer-member-initializer, *-uppercase-literal-suffix,*-diagnostic-uninitialized)
// clang-format on
#include "Vulktrt/Buffer.hpp"

namespace lve {
    void Buffer::rename(const std::string &newObjectName) {
        lveDevice.setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, BC_UI64T(memory), FORMAT("{} Memory", newObjectName).c_str());
        lveDevice.setObjectName(VK_OBJECT_TYPE_BUFFER, BC_UI64T(buffer), newObjectName.c_str());
    }

    /**
     * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
     *
     * @param instanceSize The size of an instance
     * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
     * minUniformBufferOffsetAlignment)
     *
     * @return VkResult of the buffer mapping call
     */
    VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
        if(minOffsetAlignment > 0) { return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1); }
        return instanceSize;
    }

    Buffer::Buffer(Device &device, VkDeviceSize instanceSizein, uint32_t instanceCountin, VkBufferUsageFlags usageFlagsin,
                   VkMemoryPropertyFlags memoryPropertyFlagsin, const std::string &objectName, VkDeviceSize minOffsetAlignment)
      : lveDevice{device}, instanceCount{instanceCountin}, instanceSize{instanceSizein}, usageFlags{usageFlagsin},
        memoryPropertyFlags{memoryPropertyFlagsin} {
        alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
        bufferSize = alignmentSize * instanceCount;
        // static int bufferIndex = 0;
        device.createBuffer(bufferSize, usageFlagsin, memoryPropertyFlagsin, buffer, memory);
        lveDevice.setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, BC_UI64T(memory), FORMAT("{} Memory", objectName).c_str());
        lveDevice.setObjectName(VK_OBJECT_TYPE_BUFFER, BC_UI64T(buffer), FORMAT("{}", objectName).c_str());
    }

    Buffer::~Buffer() {
        unmap();
        vkDestroyBuffer(lveDevice.device(), buffer, nullptr);
        vkFreeMemory(lveDevice.device(), memory, nullptr);
    }

    /**
     * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
     *
     * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
     * buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the buffer mapping call
     */
    VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(buffer && memory && "Called map on buffer before create");
        return vkMapMemory(lveDevice.device(), memory, offset, size, 0, &mapped);
    }

    /**
     * Unmap a mapped memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void Buffer::unmap() {
        if(mapped) {
            vkUnmapMemory(lveDevice.device(), memory);
            mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the mapped buffer. Default value writes whole buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of mapped region
     *
     */
    void Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        assert(mapped && "Cannot copy to unmapped buffer");

        if(size == VK_WHOLE_SIZE) {
            memcpy(mapped, data, bufferSize);
        } else {
            char *memOffset = std::bit_cast<char *>(mapped);
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a memory range of the buffer to make it visible to the device
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
     * complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the flush call
     */
    VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(lveDevice.device(), 1, &mappedRange);
    }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
     * the complete buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the invalidate call
     */
    VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(lveDevice.device(), 1, &mappedRange);
    }

    /**
     * Create a buffer info descriptor
     *
     * @param size (Optional) Size of the memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{buffer, offset, size};
    }

    /**
     * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset calculation
     *
     */
    void Buffer::writeToIndex(void *data, int index) { writeToBuffer(data, instanceSize, C_UL(index) * alignmentSize); }

    /**
     *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
     *
     * @param index Used in offset calculation
     *
     */
    VkResult Buffer::flushIndex(int index) { return flush(alignmentSize, C_UL(index) * alignmentSize); }

    /**
     * Create a buffer info descriptor
     *
     * @param index Specifies the region given by index * alignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo Buffer::descriptorInfoForIndex(int index) { return descriptorInfo(alignmentSize, C_UL(index) * alignmentSize); }

    /**
     * Invalidate a memory range of the buffer to make it visible to the host
     *
     * @note Only required for non-coherent memory
     *
     * @param index Specifies the region to invalidate: index * alignmentSize
     *
     * @return VkResult of the invalidate call
     */
    VkResult Buffer::invalidateIndex(int index) { return invalidate(alignmentSize, C_UL(index) * alignmentSize); }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner,*-easily-swappable-parameters, *-prefer-member-initializer, *-uppercase-literal-suffix,*-diagnostic-uninitialized)
// clang-format on
