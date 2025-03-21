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

#include "Vulktrt/KeyboardMovementController.hpp"
#include <Vulktrt/FPSCounter.hpp>

namespace lve {
    DISABLE_WARNINGS_PUSH(26432 26447)
    FirstApp::FirstApp() noexcept { loadGameObjects(); }

    FirstApp::~FirstApp() = default;
    DISABLE_WARNINGS_POP()

    static inline constexpr auto fovr = glm::radians(50.f);

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
        Camera camera{};
        camera.setViewTarget(glm::vec3{-1.f, -2.f, 2.f}, glm::vec3{0.f, 0.f, 2.5f});
        auto viewerObject = GameObject::createGameObject();
        KeyboardMovementController cameraController{};

        FPSCounter fpsCounter{lveWindow.getGLFWWindow(), wtile};
        while(!lveWindow.shouldClose()) [[likely]] {
            glfwPollEvents();
            fpsCounter.frameInTitle(false, false);
            auto frameTime = C_F(fpsCounter.getFrameTime());
            cameraController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(fovr, aspect, 0.1f, 10.f);
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
        Model::Builder modelBuilder{};
        auto whiteColor = glm::vec3{.9f, .9f, .9f};
        auto yellowColor = glm::vec3{.8f, .8f, .1f};
        auto orangeColor = glm::vec3{.9f, .6f, .1f};
        auto redColor = glm::vec3{.8f, .1f, .1f};
        auto blueColor = glm::vec3{.1f, .1f, .8f};
        auto greenColor = glm::vec3{.1f, .8f, .1f};

        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, whiteColor},
            {{-.5f, .5f, .5f}, whiteColor},
            {{-.5f, -.5f, .5f}, whiteColor},
            {{-.5f, .5f, -.5f}, whiteColor},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, yellowColor},
            {{.5f, .5f, .5f}, yellowColor},
            {{.5f, -.5f, .5f}, yellowColor},
            {{.5f, .5f, -.5f}, yellowColor},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, orangeColor},
            {{.5f, -.5f, .5f}, orangeColor},
            {{-.5f, -.5f, .5f}, orangeColor},
            {{.5f, -.5f, -.5f}, orangeColor},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, redColor},
            {{.5f, .5f, .5f}, redColor},
            {{-.5f, .5f, .5f}, redColor},
            {{.5f, .5f, -.5f}, redColor},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, blueColor},
            {{.5f, .5f, 0.5f}, blueColor},
            {{-.5f, .5f, 0.5f}, blueColor},
            {{.5f, -.5f, 0.5f}, blueColor},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, greenColor},
            {{.5f, .5f, -0.5f}, greenColor},
            {{-.5f, .5f, -0.5f}, greenColor},
            {{.5f, -.5f, -0.5f}, greenColor},

        };
        for(auto &v : modelBuilder.vertices) { v.position += offset; }

        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
        return std::make_unique<Model>(device, modelBuilder);
    }

    void FirstApp::loadGameObjects() {
        std::shared_ptr<Model> lveModel = createCubeModel(lveDevice, {.0f, .0f, .0f});
        auto cube = GameObject::createGameObject();
        cube.model = lveModel;
        cube.transform.translation = {.0f, .0f, 2.5f};
        cube.transform.scale = {.5f, .5f, .5f};

        gameObjects.push_back(std::move(cube));
    }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto, *-pro-type-union-access)
// clang-format on