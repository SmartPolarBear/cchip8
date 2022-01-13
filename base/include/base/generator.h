//
// Created by cleve on 1/13/2022.
//

#pragma once

// switch to non-experimental version when possible
#include <experimental/generator>

namespace cchip8::base
{
template<typename T, typename A = std::allocator<char>>
using generator = std::experimental::generator<T, A>;
}