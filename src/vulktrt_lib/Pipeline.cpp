/*
 * Created by gbian on 10/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */

// NOLINTBEGIN(*-include-cleaner, *-signed-bitwise)
#include "Vulktrt/Pipeline.hpp"

namespace lve {
    Pipeline::Pipeline(const std::string &vertFilepath, const std::string &fragFilepath) {
        createGraphicsPipeline(vertFilepath, fragFilepath);
    }

    std::vector<char> Pipeline::readFile(const std::string &filepath) {
        std::ifstream file{filepath, std::ios::ate | std::ios::binary};
        if(!file.is_open()) { throw std::runtime_error("failed to open file: " + filepath); }

        size_t fileSize = C_LL(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();
        return buffer;
    }

    void Pipeline::createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath) {
        const auto vertCode = readFile(vertFilepath);
        const auto fragCode = readFile(fragFilepath);
        LINFO("Vertex Shader code size: {}", vertCode.size());
        LINFO("Fragment Shader code size: {}", fragCode.size());
    }

}  // namespace lve

// NOLINTEND(*-include-cleaner, *-signed-bitwise)