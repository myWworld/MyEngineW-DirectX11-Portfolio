#pragma once

#include <iostream>
#include <mutex>
#include <utility>

namespace VideoLog
{
    inline std::mutex gMutex;

    template <typename... Args>
    void Print(Args&&... args)
    {
        std::lock_guard<std::mutex> lock(gMutex);

        ( std::cout << ... << std::forward<Args>(args));

        std::cout << '\n';
    }
}