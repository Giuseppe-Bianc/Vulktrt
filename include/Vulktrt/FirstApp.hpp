/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "Device.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"
#include "headers.hpp"
#include "Model.hpp"

namespace lve {
    class FirstApp {
    public:
        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp &other) = delete;
        FirstApp &operator=(const FirstApp &other) = delete;

        void run();

    private:
        void loadModels();
        void createPipelineLayout();
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();

        Window lveWindow{wwidth, wheight, wtile};
        Device lveDevice{lveWindow};
        SwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
        /*Pipeline lvePipeline{lveDevice, calculateRelativePathToShaders(curentP, "simple_shader.vert.opt.rmp.spv").string(),
                             calculateRelativePathToShaders(curentP, "simple_shader.frag.opt.rmp.spv").string(),
                             Pipeline::defaultPipelineConfigInfo(wwidth, wheight)};*/
        std::unique_ptr<Pipeline> lvePipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
        std::unique_ptr<Model> lveModel;
    };
}  // namespace lve

// NOLINTEND(*-include-cleaner)