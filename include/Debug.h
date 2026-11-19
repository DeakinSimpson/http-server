//
// Created by Deakin Simpson on 19/11/2026.
//

#pragma once

#include <iostream>

#ifdef NDEBUG
#define DEBUG(x)
#else
#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#endif
