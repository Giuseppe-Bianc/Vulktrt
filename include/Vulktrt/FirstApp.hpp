/*
 * Created by gbian on 09/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "Descriptors.hpp"
#include "Device.hpp"
#include "GameObject.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "headers.hpp"

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

        Window lveWindow{wwidth, wheight, wtile};
        Device lveDevice{lveWindow};
        Renderer lveRenderer{lveWindow, lveDevice};
        std::unique_ptr<DescriptorPool> globalPool{};
        GameObject::Map gameObjects;
    };
    DISABLE_WARNINGS_POP()
}  // namespace lve

// NOLINTEND(*-include-cleaner)
