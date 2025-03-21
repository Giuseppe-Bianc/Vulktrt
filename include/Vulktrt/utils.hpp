/*
 * Created by gbian on 21/03/2025.
 * Copyright (c) 2025 All rights reserved.
 */

#pragma once

#include "headers.hpp"

namespace lve {

    // from: https://stackoverflow.com/a/57595105
    template <typename T, typename... Rest> void hashCombine(std::size_t &seed, const T &v, const Rest &...rest) {
        seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hashCombine(seed, rest), ...);
    };

}  // namespace lve