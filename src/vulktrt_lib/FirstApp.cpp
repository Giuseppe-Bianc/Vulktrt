/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto, *-pro-type-union-access)
// clang-format on
#include "Vulktrt/FirstApp.hpp"
#include "Vulktrt/Camera.hpp"
#include "Vulktrt/SimpleRenderSystem.hpp"

#include <Vulktrt/FPSCounter.hpp>

namespace lve {
    DISABLE_WARNINGS_PUSH(26432 26447)
    FirstApp::FirstApp() noexcept { loadGameObjects(); }

    FirstApp::~FirstApp() = default;
    DISABLE_WARNINGS_POP()

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
        Camera camera{};
        camera.setOrthographicProjection(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
        FPSCounter fpsCounter{lveWindow.getGLFWWindow(), wtile};
        while(!lveWindow.shouldClose()) [[likely]] {
            fpsCounter.frameInTitle(false, false);
            glfwPollEvents();
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    std::unique_ptr<Model> createCubeModel(Device &device, glm::vec3 offset) {
        auto whiteColor = glm::vec3{.9f, .9f, .9f};
        auto yellowColor = glm::vec3{.8f, .8f, .1f};
        auto orangeColor = glm::vec3{.9f, .6f, .1f};
        auto redColor = glm::vec3{.8f, .1f, .1f};
        auto blueColor = glm::vec3{.1f, .1f, .8f};
        auto greenColor = glm::vec3{.1f, .8f, .1f};

        std::vector<Model::Vertex> vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, whiteColor},
            {{-.5f, .5f, .5f}, whiteColor},
            {{-.5f, -.5f, .5f}, whiteColor},
            {{-.5f, -.5f, -.5f}, whiteColor},
            {{-.5f, .5f, -.5f}, whiteColor},
            {{-.5f, .5f, .5f}, whiteColor},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, yellowColor},
            {{.5f, .5f, .5f}, yellowColor},
            {{.5f, -.5f, .5f}, yellowColor},
            {{.5f, -.5f, -.5f}, yellowColor},
            {{.5f, .5f, -.5f}, yellowColor},
            {{.5f, .5f, .5f}, yellowColor},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, orangeColor},
            {{.5f, -.5f, .5f}, orangeColor},
            {{-.5f, -.5f, .5f}, orangeColor},
            {{-.5f, -.5f, -.5f}, orangeColor},
            {{.5f, -.5f, -.5f}, orangeColor},
            {{.5f, -.5f, .5f}, orangeColor},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, redColor},
            {{.5f, .5f, .5f}, redColor},
            {{-.5f, .5f, .5f}, redColor},
            {{-.5f, .5f, -.5f}, redColor},
            {{.5f, .5f, -.5f}, redColor},
            {{.5f, .5f, .5f}, redColor},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, blueColor},
            {{.5f, .5f, 0.5f}, blueColor},
            {{-.5f, .5f, 0.5f}, blueColor},
            {{-.5f, -.5f, 0.5f}, blueColor},
            {{.5f, -.5f, 0.5f}, blueColor},
            {{.5f, .5f, 0.5f}, blueColor},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, greenColor},
            {{.5f, .5f, -0.5f}, greenColor},
            {{-.5f, .5f, -0.5f}, greenColor},
            {{-.5f, -.5f, -0.5f}, greenColor},
            {{.5f, -.5f, -0.5f}, greenColor},
            {{.5f, .5f, -0.5f}, greenColor},

        };
        for(auto &v : vertices) { v.position += offset; }
        return std::make_unique<Model>(device, vertices);
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<Model> lveModel = createCubeModel(lveDevice, {.0f, .0f, .0f});
        auto cube = GameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = {.0f, .0f, .5f};
        cube.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube));
    }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto, *-pro-type-union-access)
// clang-format on