/*
 * Created by gbian on 04/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */
// NOLINTBEGIN(*-include-cleaner, *-avoid-magic-numbers,*-magic-numbers)
#include "Vulktrt/FPSCounter.hpp"

FPSCounter::FPSCounter(GLFWwindow *window, std::string_view title) noexcept
  : last_time(vnd::clock::now()), frames(0), fps(0.0L), ms_per_frame(0.0L), m_window(window), m_title(title) {}

void FPSCounter::frame(bool vsync, bool showMax) {
    updateFPS();
    if(showMax) {
        LINFO("{:.3LF} fps/{} - Max: {:.3LF} - VSync: {}", fps, ms_per_frameComposition, max_fps, vsync ? "Enabled" : "Disabled");
    } else {
        LINFO("{:.3LF} fps/{} - VSync: {}", fps, ms_per_frameComposition, vsync ? "Enabled" : "Disabled");
    }
}

void FPSCounter::frameInTitle(bool vsync, bool showMax) {
    updateFPS();
    if(showMax) {
        glfwSetWindowTitle(m_window, FORMATST("{} - {:.3LF} fps/{} - Max: {:.3LF} - VSync: {}", m_title, fps, ms_per_frameComposition,
                                              max_fps, vsync ? "Enabled" : "Disabled")
                                         .c_str());
    } else {
        glfwSetWindowTitle(
            m_window,
            FORMATST("{} - {:.3LF} fps/{} - VSync: {}", m_title, fps, ms_per_frameComposition, vsync ? "Enabled" : "Disabled").c_str());
    }
}

std::string FPSCounter::transformTime(const long double inputTimeMilli) noexcept {
    using namespace std::chrono;

    const auto durationmils = duration<long double, std::milli>(inputTimeMilli);

    // Use floor to extract whole milliseconds and microseconds.
    const auto durationMs = floor<milliseconds>(durationmils);
    const auto remainderAfterMs = durationmils - durationMs;
    const auto durationUs = floor<microseconds>(remainderAfterMs);
    const auto remainderAfterUs = remainderAfterMs - durationUs;
    // Round the final nanoseconds from the remaining fraction.
    const auto durationNs = round<nanoseconds>(remainderAfterUs);

    return FORMAT("{}ms,{}us,{}ns", C_LD(durationMs.count()), C_LD(durationUs.count()), C_LD(durationNs.count()));
}

void FPSCounter::updateFPS() noexcept {
    frames++;
    const auto ldframes = C_LD(frames);
    const auto current_time = vnd::clock::now();
    const auto time_step = vnd::TimeValues(ch::duration_cast<vnd::nanolld>(current_time - last_time).count());
    if(const auto &time_steps = time_step.get_seconds(); time_steps >= 1.0L) {
        last_time = current_time;
        fps = ldframes / time_steps;
        ms_per_frame = time_step.get_millis() / ldframes;
        frames = 0;
        max_fps = std::max(max_fps, fps);
    }
    ms_per_frameComposition = transformTime(ms_per_frame);
}

long double FPSCounter::getFPS() const noexcept { return fps; }

long double FPSCounter::getMsPerFrame() const noexcept { return ms_per_frame; }

// NOLINTEND(*-include-cleaner, *-avoid-magic-numbers,*-magic-numbers)