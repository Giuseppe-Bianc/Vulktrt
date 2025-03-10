/*
* Created by gbian on 10/03/2025.
* Copyright (c) 2025 All rights reserved.
*/
// NOLINTBEGIN(*-include-cleaner)
#pragma once

#include "headers.hpp"

namespace lve {
    class Pipeline {
    public:
        Pipeline(const std::string &vertFilepath, const std::string &fragFilepath);

    private:
        static std::vector<char> readFile(const std::string &filepath);

        void createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath);
    };
} // lve

// NOLINTEND(*-include-cleaner)