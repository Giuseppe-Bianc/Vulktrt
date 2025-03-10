/*
 * Created by gbian on 03/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#include <Vulktrt/Window.hpp>
#include <Vulktrt/vulkanCheck.hpp>

namespace lve {
    std::string_view get_glfw_error_name(int error_code) {
        switch(error_code) {
        case GLFW_NOT_INITIALIZED:
            return "GLFW_NOT_INITIALIZED";
        case GLFW_NO_CURRENT_CONTEXT:
            return "GLFW_NO_CURRENT_CONTEXT";
        case GLFW_INVALID_ENUM:
            return "GLFW_INVALID_ENUM";
        case GLFW_INVALID_VALUE:
            return "GLFW_INVALID_VALUE";
        case GLFW_OUT_OF_MEMORY:
            return "GLFW_OUT_OF_MEMORY";
        case GLFW_API_UNAVAILABLE:
            return "GLFW_API_UNAVAILABLE";
        case GLFW_VERSION_UNAVAILABLE:
            return "GLFW_VERSION_UNAVAILABLE";
        case GLFW_PLATFORM_ERROR:
            return "GLFW_PLATFORM_ERROR";
        case GLFW_FORMAT_UNAVAILABLE:
            return "GLFW_FORMAT_UNAVAILABLE";
        case GLFW_NO_WINDOW_CONTEXT:
            return "GLFW_NO_WINDOW_CONTEXT";
        default:
            return "UNKNOWN_ERROR";
        }
    }

    void errorCallback(int error, const char *description) {
        auto errorName = get_glfw_error_name(error);
        LINFO("GLFW Error: {} ({}) - {}", error, errorName, description);
    }

    void keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
        switch(key) {
        // NOLINT(*-multiway-paths-covered)
        case GLFW_KEY_ESCAPE:
            if(action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                LINFO("Escape key pressed, closing window.");
            }
            break;
            [[likely]] default:
            // Handle other keys here
            break;
        }
    }

    Window::Window(const int w, const int h, const std::string_view &window_name) noexcept
        : width(w), height(h), windowName(window_name) { initWindow(); }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void Window::initWindow() {
        initializeGLFW();
        setHints();
        createWindow();
        centerWindow();
    }

    void Window::createWindow() {
        vnd::AutoTimer timer("window creation");

        // Use std::unique_ptr with custom deleter
        window = glfwCreateWindow(width, height, windowName.data(), nullptr, nullptr);

        if(!window) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window.");
        }
        glfwSetKeyCallback(window, keyCallback);
    }

    void Window::setHints() const {
        vnd::AutoTimer timer("set glfw hints");
        // Set GLFW context version and profile
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
        // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    }

    void Window::initializeGLFW() {
        vnd::AutoTimer timer("glfw setup");
        if(!glfwInit()) {
            LCRITICAL("Failed to initialize GLFW");
            throw std::runtime_error("Failed to initialize GLFW.");
        }
        if(!glfwVulkanSupported()) {
            glfwTerminate();
            LCRITICAL("Failed to initialize GLFW. Vulkan not supported");
            throw std::runtime_error("Failed to initialize GLFW. Vulkan not supported");
        }
        glfwSetErrorCallback(errorCallback);
    }

    std::string Window::formatMode(const GLFWvidmode *mode) const {
        return FORMAT("({}x{}, Bits rgb{}{}{}, RR:{}Hz)", mode->width, mode->height, mode->redBits, mode->greenBits, mode->blueBits,
                      mode->refreshRate);
    }

    void Window::centerWindow() {
        vnd::Timer monitort("get primary Monitor");
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        if(!primaryMonitor) { throw std::runtime_error("Failed to get the primary monitor."); }
        LINFO("{}", monitort);

        vnd::Timer modet("get mode");
        const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
        if(!mode) { throw std::runtime_error("Failed to get the video mode of the primary monitor."); }
        LINFO("{}", modet);

        vnd::Timer crepositiont("calculating for reposition");
        const int monitorWidth = mode->width;
        const int monitorHeight = mode->height;
        int windowWidth;
        int windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        auto centerX = CALC_CENTRO(monitorWidth, windowWidth);
        auto centerY = CALC_CENTRO(monitorHeight, windowHeight);
        LINFO("{}", crepositiont);

        vnd::Timer wrepositiont("window reposition");
        glfwSetWindowPos(window, centerX, centerY);
        int posX = 0;
        int posY = 0;
        glfwGetWindowPos(window, &posX, &posY);
        if(posX != centerX || posY != centerY) { throw std::runtime_error("Failed to position the window at the center."); }
        // LINFO("{}", wrepositiont);
        int xPos;
        int yPos;
        float xScale;
        float yScale;
        int monitorPhysicalWidth;
        int monitorPhysicalHeight;
        vnd::Timer tmonitorinfo("get monitor info");
        glfwGetMonitorPos(primaryMonitor, &xPos, &yPos);
        glfwGetMonitorContentScale(primaryMonitor, &xScale, &yScale);
        glfwGetMonitorPhysicalSize(primaryMonitor, &monitorPhysicalWidth, &monitorPhysicalHeight);
        LINFO("{}", tmonitorinfo);
        glfwSetWindowUserPointer(window, this);
        glfwShowWindow(window);
        LINFO("Monitor:\"{}\", Phys:{}x{}mm, Scale:({}/{}), Pos:({}/{})", glfwGetMonitorName(primaryMonitor), monitorPhysicalWidth,
              monitorPhysicalHeight, xScale, yScale, xPos, yPos);
        LINFO("Monitor Mode:{}", formatMode(mode));
        LINFO("Created the window {0}: (w: {1}, h: {2}, pos:({3}/{4}))", windowName.data(), width, height, centerX, centerY);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
        VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, surface), "failed to craete window surface");
    }
} // namespace lve

// NOLINTEND(*-include-cleaner)