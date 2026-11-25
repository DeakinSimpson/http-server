//
// Created by Deakin Simpson on 20/11/2026.
//

#pragma once
#include <functional>
#include <iostream>
#include <utility>
#include <cerrno>
#include <cstring>
#include <string>
#include <system_error>

namespace http_server
{
    template <typename F, typename... Args>
    static auto system_error_handle(F&& func,
        const std::string& error_msg, Args&&... args)
    {
        /**
         * i was getting some really wiered errors, so i needed to use
         * std::invoke to call the function, none of the errors actually
         * caused runtime or compiletime errors but clang did not like it
         */
        const auto status {
            std::invoke(std::forward<F>(func),
            std::forward<Args>(args)...)
        };

        if (status == -1)
        {
            const int error_code { errno };
            std::cerr << error_msg << ": " << strerror(error_code) << '\n';

            throw std::system_error(
                error_code,
                std::generic_category(),
                error_msg
            );
        }

        return status;
    }
}
