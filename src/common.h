#pragma once
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <charconv>
#include <cstring>
#include <functional>
#include "parser.hpp"

#define log std::cerr<<
#define COLOR3(R, G, B) {R, G, B, 255}

#ifdef __GNUC__
#   define LD_NOINLINE [[gnu::noinline]] inline
#elifdef __clang__
#   define LD_NOINLINE [[clang::noinline]] inline
#elifdef _MSC_VER
#   define LD_NOINLINE __declspec(noinline) inline
#else
#   define LD_NOINLINE
#endif

constexpr double inv_sqrt2 = 1.0f / std::numbers::sqrt2_v<double>;

using f32 = float;
using uint32 = uint32_t;
using usize = size_t;
using string = std::string;
using strview = std::string_view;

namespace G {
    inline f32 dt;
    inline f32 fps;
    inline uint32  max_fps = 99'999lu;

    inline constexpr uint32 ini_win_w = 1280;
    inline constexpr uint32 ini_win_h = 720;
    inline uint32 win_w = ini_win_w;
    inline uint32 win_h = ini_win_h;
    inline const char* win_title = "";
};


struct Animation {
    using Clock = std::chrono::steady_clock;

    f32 from, to;
    f32 duration;
    bool loop = false;
    std::function<void()> resetter = {};

    Clock::time_point started_at;

    Animation(f32 from, f32 to, f32 duration_ms, bool loop = false):
        from(from), to(to),
        duration(duration_ms), loop(loop),
        started_at(Clock::now())
    {}

    void make(f32& from, f32 to, f32 duration_ms, bool loop = false) {
        *this = Animation(from, to, duration_ms, loop);
    }

    f32 value() const {
        auto now = Clock::now();
        f32 elapsed = std::chrono::duration<f32, std::milli>(now - started_at).count();
        f32 alpha = elapsed / duration;
        if (loop) alpha = std::fmod(alpha, 2.0f); // 0→1 grow, 1→2 shrink
        if (alpha > 1.0f) alpha = 2.0f - alpha;   // mirror back
        alpha = std::min(alpha, 1.0f);
        return from + (to - from) * alpha;
    }

    void defer(std::function<void()> resetter) {
        this->resetter = resetter;
    }

    bool done() {
        auto now = Clock::now();
        f32 elapsed = std::chrono::duration<f32, std::milli>(now - started_at).count();
        if (loop) return elapsed >= duration * 2.0f; // full there-and-back
        return elapsed >= duration;
    }

};

