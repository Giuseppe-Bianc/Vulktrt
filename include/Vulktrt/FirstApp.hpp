/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */

#pragma once

#include "Pipeline.hpp"
#include "Window.hpp"
#include "headers.hpp"

namespace lve {
    class FirstApp {
    public:
        void run();

    private:
        Window lveWindow{wwidth, wheight, wtile};
        Pipeline lvePipeline{calculateRelativePathToShaders(curentP, "simple_shader.vert.opt.rmp.spv").string(),
                             calculateRelativePathToShaders(curentP, "simple_shader.frag.opt.rmp.spv").string()};
    };
}  // namespace lve