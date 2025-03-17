/*
 * Created by derbinan on 14/03/25.
 * Copyright (c) 2025 All rights reserved.
 */

#pragma once

#include "headers.hpp"

namespace lve {
    class Monitor {
    public:
        explicit Monitor(GLFWmonitor *monitorin);
        void fetchMonitorInfo() noexcept;
        std::string formatMode() const;
        int getWidth() const noexcept { return monitorWidth; }
        int getHeight() const noexcept { return monitorHeight; }
        int getPhysicalWidth() const noexcept { return physicalWidth; }
        int getPhysicalHeight() const noexcept { return physicalHeight; }
        float getScaleX() const noexcept { return scaleX; }
        float getScaleY() const noexcept { return scaleY; }
        int getXPos() const noexcept { return xPos; }
        int getYPos() const noexcept { return yPos; }

    private:
        GLFWmonitor *monitor;
        const GLFWvidmode *mode;
        int monitorWidth, monitorHeight;
        int physicalWidth, physicalHeight;
        float scaleX, scaleY;
        int xPos, yPos;
    };
}  // namespace lve
