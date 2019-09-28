#include "server/perf_watcher.hpp"

#include <spdlog/spdlog.h>

namespace blyss::server
{
    const uint64_t frame_leeway = 10;

    perf_watcher::perf_watcher(uv_loop_t* loop, std::uint64_t ms_per_frame, std::uint64_t slow_warning_reset_ms)
        : loop_{loop}
        , ms_per_frame_{ms_per_frame}
        , previous_time_{ uv_now(loop_) }
        , slow_warning_reset_ms_{slow_warning_reset_ms}
    {
        uv_timer_init(loop_, &show_warning_timer_);
    }

    void perf_watcher::start()
    {
        self_ptr_ = shared_from_this();
        show_warning_timer_.data = static_cast<void*>(&self_ptr_);
        reset();
    }

    void perf_watcher::update()
    {
        const auto current_time = uv_now(loop_);
        const auto diff = current_time - previous_time_;
        previous_time_ = current_time;

        if (show_slow_warning_ && diff > ms_per_frame_ + frame_leeway)
        {
            show_message(diff - ms_per_frame_);
        }
    }

    void perf_watcher::reset()
    {
        show_slow_warning_ = true;
    }

    void perf_watcher::show_message(const std::uint64_t missed_ms)
    {
        spdlog::warn("Server is running {0:d} ms behind! Consider getting a faster cpu...", missed_ms);
        show_slow_warning_ = false;
        uv_timer_start(&show_warning_timer_, [](uv_timer_t* handle)
        {
            const auto scope = static_cast<std::weak_ptr<perf_watcher>*>(handle->data);
            if (auto s = (*scope).lock())
            {
                s->reset();
            }
        }, slow_warning_reset_ms_, 0);
    }

}