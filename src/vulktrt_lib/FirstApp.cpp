/*
* Created by gbian on 09/03/2025.
* Copyright (c) 2025 All rights reserved.
*/
// NOLINTBEGIN(*-include-cleaner)
#include "Vulktrt/FirstApp.hpp"

#include <Vulktrt/FPSCounter.hpp>

namespace lve {
    void FirstApp::run() {
        FPSCounter fpsCounter{lveWindow.getGLFWWindow(), wtile};
        while(!lveWindow.shouldClose()) {
            fpsCounter.frameInTitle(false, false);
            //lveWindow.swapBuffers();
            // Take care of all GLFW events
            glfwPollEvents();
        }
    }
} // lve

// NOLINTEND(*-include-cleaner)