/*
 * Created by gbian on 03/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "WindowCallback.hpp"
#include "headers.hpp"
#include <string>
#include <string_view>

namespace lve {

    // static void framebufferResizeCallback(GLFWwindow *window, int width, int height) noexcept;
    // NOLINT(*-special-member-functions)
    class Window {
    public:
        Window(const int w, const int h, const std::string_view &window_name) noexcept;
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        [[nodiscard]] GLFWwindow *getGLFWWindow() const noexcept { return window; }
        [[nodiscard]] bool shouldClose() const noexcept { return glfwWindowShouldClose(window); }
        //[[nodiscard]] bool wasWindowResized() noexcept { return framebufferResized; }
        void resetWindowResizedFlag() noexcept { framebufferResized = false; }
        // void swapBuffers() const noexcept { glfwSwapBuffers(window); }
        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        VkExtent2D getExtent() { return {C_UI32T(width), C_UI32T(height)}; }

        static void initializeGLFW();

    private:
        void initWindow();

        void createWindow();

        void setHints() const;

        [[nodiscard]] std::string formatMode(const GLFWvidmode *mode) const;

        void centerWindow();

        int width;
        int height;
        bool framebufferResized = false;
        std::string_view windowName;
        GLFWwindow *window{nullptr};
    };
}  // namespace lve

// NOLINTEND(*-include-cleaner)
