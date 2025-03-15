#pragma once

// NOLINTBEGIN(*-include-cleaner)
#include "headers.hpp"

// Disabilita l'avviso unused-function
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

inline std::string_view get_glfw_error_name(int error_code) {
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

inline static void errorCallback(int error, const char *description) {
    auto errorName = get_glfw_error_name(error);
    LINFO("GLFW Error: {} ({}) - {}", error, errorName, description);
}

inline static void keyCallback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
    switch(key) {
    // NOLINTN(*-multiway-paths-covered)
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

// Ripristina le impostazioni degli avvisi
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

// NOLINTEND(*-include-cleaner)