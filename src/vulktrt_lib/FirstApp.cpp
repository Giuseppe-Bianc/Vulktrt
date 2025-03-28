/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// clang-format off
// NOLINTBEGIN(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto, *-pro-type-union-access)
// clang-format on
#include "Vulktrt/FirstApp.hpp"
#include "Vulktrt/Buffer.hpp"
#include "Vulktrt/Camera.hpp"
#include "Vulktrt/SimpleRenderSystem.hpp"

#include "Vulktrt/KeyboardMovementController.hpp"
#include <Vulktrt/FPSCounter.hpp>

namespace lve {
    DISABLE_WARNINGS_PUSH(4324)
    struct GlobalUBO {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f};  // w is intensity
        glm::vec3 lightPosition{-1.f};
        alignas(16) glm::vec4 lightColor{1.f};  // w is light intensity
    };
    DISABLE_WARNINGS_POP()

    DISABLE_WARNINGS_PUSH(26432 26447)

    FirstApp::FirstApp() noexcept {
        globalPool = DescriptorPool::Builder(lveDevice)
                         .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                         .build();
        lveDevice.setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, BC_UI64T(globalPool->getDescriptorPool()), "Global Descriptor Pool");
        loadGameObjects();
    }

    FirstApp::~FirstApp() = default;
    DISABLE_WARNINGS_POP()

    static inline constexpr auto GlobalUBOsize = sizeof(GlobalUBO);
    static inline constexpr auto fovr = glm::radians(50.f);

    void FirstApp::run() {
        std::vector<std::unique_ptr<Buffer>> uboBuffers{SwapChain::MAX_FRAMES_IN_FLIGHT};
        for(size_t i = 0; i < uboBuffers.size(); ++i) {
            uboBuffers[i] = std::make_unique<Buffer>(lveDevice, GlobalUBOsize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, FORMAT("Global UBO {}", i));
            uboBuffers[i]->map();
        }
        auto globalSetLayout =
            DescriptorSetLayout::Builder(lveDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL).build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(size_t i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
            lveDevice.setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, BC_UI64T(globalDescriptorSets[i]),
                                    FORMAT("Global Descriptor Set {}", i).c_str());
        }

        lveDevice.setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, BC_UI64T(globalSetLayout->getDescriptorSetLayout()),
                                "Global Descriptor Set Layout");
        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        Camera camera{};
        camera.setViewTarget(glm::vec3{-1.f, -2.f, 2.f}, glm::vec3{0.f, 0.f, 2.5f});
        auto viewerObject = GameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;
        KeyboardMovementController cameraController{};

        FPSCounter fpsCounter{lveWindow.getGLFWWindow(), wtile};
        while(!lveWindow.shouldClose()) [[likely]] {
            glfwPollEvents();
            fpsCounter.frameInTitle(false, false);
            auto frameTime = C_F(fpsCounter.getFrameTime());
            cameraController.moveInPlaneXZ(lveWindow.getGLFWWindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            float aspect = lveRenderer.getAspectRatio();
            camera.setPerspectiveProjection(fovr, aspect, 0.1f, 100.f);
            if(auto commandBuffer = lveRenderer.beginFrame()) {
                auto frameIndex = C_ST(lveRenderer.getFrameIndex());
                FrameInfo frameInfo{C_I(frameIndex), frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};
                GlobalUBO ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects() {
        const auto scalevector = glm::vec3{3.f, 1.5f, 3.f};
        std::shared_ptr<Model> lveModel = Model::createModelFromFile(lveDevice,
                                                                     calculateRelativePathToModels(curentP, "flat_vase.obj").string());
        auto flatVase = GameObject::createGameObject();
        flatVase.model = lveModel;
        flatVase.transform.translation = {-.5f, .5f, 0.f};
        flatVase.transform.scale = scalevector;

        lveModel = Model::createModelFromFile(lveDevice, calculateRelativePathToModels(curentP, "smooth_vase.obj").string());
        auto smoothVase = GameObject::createGameObject();
        smoothVase.model = lveModel;
        smoothVase.transform.translation = {.5f, .5f, 0.f};
        smoothVase.transform.scale = scalevector;
        lveModel = Model::createModelFromFile(lveDevice, calculateRelativePathToModels(curentP, "quad.obj").string());
        auto floor = GameObject::createGameObject();
        floor.model = lveModel;
        floor.transform.translation = {.5f, .5f, 0.f};
        floor.transform.scale = {3.f, 1.f, 3.f};
        gameObjects.emplace(flatVase.getId(), std::move(flatVase));
        gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));
        gameObjects.emplace(floor.getId(), std::move(floor));
    }
}  // namespace lve

// clang-format off
// NOLINTEND(*-include-cleaner, *-diagnostic-missing-braces, *-avoid-magic-numbers,*-magic-numbers, *-uppercase-literal-suffix, *-uppercase-literal-suffix, *-pro-type-member-init,*-member-init, *-qualified-auto, *-pro-type-union-access)
// clang-format on