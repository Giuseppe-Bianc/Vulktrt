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
    struct GlobalUBO {
        glm::mat4 ProjectionView{1.f};
        glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    };

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
        vnd::Timer timer{"Global Descriptor Set Layout"};
        std::vector<std::unique_ptr<Buffer>> uboBuffers{SwapChain::MAX_FRAMES_IN_FLIGHT};
        for(int i = 0; i < uboBuffers.size(); ++i) {
            uboBuffers[i] = std::make_unique<Buffer>(lveDevice, GlobalUBOsize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, FORMAT("Global UBO {}", i));
            uboBuffers[i]->map();
        }
        auto globalSetLayout =
            DescriptorSetLayout::Builder(lveDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT).build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < globalDescriptorSets.size(); i++) {
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
                int frameIndex = lveRenderer.getFrameIndex();
                FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex]};
                GlobalUBO ubo{};
                ubo.ProjectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
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