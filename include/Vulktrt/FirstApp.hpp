/*
* Created by gbian on 09/03/2025.
* Copyright (c) 2025 All rights reserved.
*/

#pragma once

#include "headers.hpp"
#include "Window.hpp"

namespace lve {
    class FirstApp {
    public:
        void run();
    private:
        Window lveWindow{wwidth, wheight, wtile};
    };
} // lve