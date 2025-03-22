/*
 * Created by gbian on 18/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */

#pragma once
#include "Camera.hpp"
#include "Device.hpp"
#include "FrameInfo.hpp"
#include "GameObject.hpp"
#include "Pipeline.hpp"

namespace lve {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(FrameInfo &frameInfo, std::vector<GameObject> &gameObjects);

    private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        Device &lveDevice;

        std::unique_ptr<Pipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
    };
}  // namespace lve
