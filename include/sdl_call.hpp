#pragma once
#include <spdlog/spdlog.h>
#include "log.hpp"
// 用于有返回值的 SDL 函数（返回 bool/int，失败返回 false/负数）
#define SDL_CALL(expr) \
    do { \
        if (!(expr)) { \
            LOG_ERROR("[SDL] {} failed: {}", #expr, SDL_GetError()); \
        } \
    } while(0)

// 用于返回指针的 SDL 函数（失败返回 nullptr）
#define SDL_CALL_PTR(ptr, expr) \
    do { \
        (ptr) = (expr); \
        if (!(ptr)) { \
            LOG_ERROR("[SDL] {} failed: {}", #expr, SDL_GetError()); \
        } \
    } while(0)
