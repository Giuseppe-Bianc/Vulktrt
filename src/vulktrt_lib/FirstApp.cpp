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

    void FirstApp::loadGameObjects() {
        const auto scalevector = glm::vec3{3.f, 1.5f, 3.f};
        std::shared_ptr<Model> lveModel = Model::createModelFromFile(lveDevice, calculateRelativePathToModels(curentP, "flat_vase.obj").string());
        auto flatVase = GameObject::createGameObject();
        flatVase.model = lveModel;
        flatVase.transform.translation = {-.5f, .5f, 2.5f};
        flatVase.transform.scale = scalevector;

        lveModel = Model::createModelFromFile(lveDevice, calculateRelativePathToModels(curentP, "smooth_vase.obj").string());
        auto smoothVase = GameObject::createGameObject();
        smoothVase.model = lveModel;
        smoothVase.transform.translation = {.5f, .5f, 2.5f};
        smoothVase.transform.scale = scalevector;
        gameObjects.push_back(std::move(flatVase));
        gameObjects.push_back(std::move(smoothVase));
    }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto, *-pro-type-union-access)
// clang-format on