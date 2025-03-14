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
        void fetchMonitorInfo();
        std::string formatMode() const;
        int getWidth() const { return monitorWidth; }
        int getHeight() const { return monitorHeight; }
        int getPhysicalWidth() const { return physicalWidth; }
        int getPhysicalHeight() const { return physicalHeight; }
        float getScaleX() const { return scaleX; }
        float getScaleY() const { return scaleY; }
        int getXPos() const { return xPos; }
        int getYPos() const { return yPos; }

    private:
        GLFWmonitor *monitor;
        const GLFWvidmode *mode;
        int monitorWidth, monitorHeight;
        int physicalWidth, physicalHeight;
        float scaleX, scaleY;
        int xPos, yPos;
    };
}  // namespace lve