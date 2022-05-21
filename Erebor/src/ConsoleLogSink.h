#pragma once

#include "Panels/ConsolePanel.h"

#include "spdlog/sinks/base_sink.h"
#include "spdlog/details/null_mutex.h"
#include <mutex>

namespace Mahakam::Editor
{
    template<typename Mutex>
    class ConsoleLogSink : public spdlog::sinks::base_sink<Mutex>
    {
    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            // log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
            // msg.raw contains pre formatted log

            // If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

            std::string formattedString = fmt::to_string(formatted);

            ConsolePanel::AddLog(formattedString);
        }

        void flush_() override
        {
            //std::cout << std::flush;
        }
    };

    using ConsoleLogSinkMt = ConsoleLogSink<std::mutex>;
    using ConsoleLogSinkSt = ConsoleLogSink<spdlog::details::null_mutex>;
}