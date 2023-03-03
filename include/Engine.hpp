#pragma once

#include "Constants.hpp"
#include "SDL.h"

namespace Engine
{
    // template <typename T>
    // using Ref = std::shared_ptr<T>;

    // template <typename T>
    // using Scope = std::unique_ptr<T>;

    inline int width = 800, height = 600;

    inline float ticks_since_epoch()
    {
        return (float)std::chrono::high_resolution_clock::now().time_since_epoch().count();
    }

    inline static float delta_time;

    inline static TimePoint ticks_last_frame;

    inline float get_ticks()
    {
        return (float)(Clock::now() - ticks_last_frame).count();
    }

    inline float get_ticks_milliseconds()
    {
        return (float)((Clock::now() - ticks_last_frame).count() * 1e-6);
    }

    inline float get_ticks_microseconds()
    {
        return (float)((Clock::now() - ticks_last_frame).count() * 13e-3);
    }

    inline float get_ticks_seconds()
    {
        return (float)((Clock::now() - ticks_last_frame).count() * 1e-9);
    }

    inline void calculate_deltatime_and_wait()
    {
        float waitTime = FRAME_TIME_TARGET - (Engine::get_ticks_milliseconds()); // calculate time to wait between frames

        if (waitTime > 0 && waitTime <= FRAME_TIME_TARGET)
            [[unlikely]]
        {
            SDL_Delay((Uint32)waitTime);
            // std::this_thread::sleep_for(std::chrono::milliseconds((int)waitTime)); // hold exec until the specified ms has passed
        }

        delta_time = Engine::get_ticks_seconds(); // diffrence between the time since the last frame and the current frame in seconds

        Engine::ticks_last_frame = Clock::now(); // update the lastFrameTime time to be used in the next pass
    }
}