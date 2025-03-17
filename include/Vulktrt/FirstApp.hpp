/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "Device.hpp"
#include "Model.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"
#include "headers.hpp"
#include "GameObject.hpp"

namespace lve {
    DISABLE_WARNINGS_PUSH(26447)
    class FirstApp {
    public:
        FirstApp() noexcept;
        ~FirstApp();

        FirstApp(const FirstApp &other) = delete;
        FirstApp &operator=(const FirstApp &other) = delete;

        void run();

    private:
        void loadGameObjects();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void freeCommandBuffers() noexcept;
        void drawFrame();
        void recreateSwapChain();
        void recordCommandBuffer(int imageIndex);
        void renderGameObjects(VkCommandBuffer commandBuffer);


        Window lveWindow{wwidth, wheight, wtile};
        Device lveDevice{lveWindow};
        // SwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
        std::unique_ptr<SwapChain> lveSwapChain;
        /*Pipeline lvePipeline{lveDevice, calculateRelativePathToShaders(curentP, "simple_shader.vert.opt.rmp.spv").string(),
                             calculateRelativePathToShaders(curentP, "simple_shader.frag.opt.rmp.spv").string(),
                             Pipeline::defaultPipelineConfigInfo(wwidth, wheight)};*/
        std::unique_ptr<Pipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<GameObject> gameObjects;
    };
    DISABLE_WARNINGS_POP()
}  // namespace lve

// NOLINTEND(*-include-cleaner)
