//
// Created by Deakin Simpson on 19/11/2026.
//

#pragma once

#include <iostream>

/**
 * this is a macro that runs only in DEBUG mode, this allows for me to add debug
 * lines that do not run in release mode
 */

#ifdef NDEBUG
#define DEBUG(x)
#else
#define DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#endif
