/*
 * Created by gbian on 04/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "headers.hpp"

class FPSCounter {
public:
    explicit FPSCounter(GLFWwindow *window, std::string_view title = "title") noexcept;
    void frame(bool vsync = false, bool showMax = true);
    void frameInTitle(bool vsync = false, bool showMax = true);
    void updateFPS() noexcept;
    [[nodiscard]] long double getFPS() const noexcept;
    [[nodiscard]] long double getFrameTime() const noexcept { return frameTime; };
    [[nodiscard]] long double getMsPerFrame() const noexcept;

private:
    [[nodiscard]] std::string transformTime(const long double inputTimeMilli) noexcept;
    vnd::time_point last_time;
    int frames;
    long double fps;
    long double max_fps = 0;
    long double ms_per_frame;
    long double frameTime{};
    long double totalTime{};
    GLFWwindow *m_window;
    std::string_view m_title;
    std::string ms_per_frameComposition;
};

// NOLINTEND(*-include-cleaner)