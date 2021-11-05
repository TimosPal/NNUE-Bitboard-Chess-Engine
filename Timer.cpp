#include "Timer.h"

#include <iostream>

namespace ChessEngine {

    Timer::Timer(std::string function_name) : function_name_(function_name) {
        starting_time_point_ = std::chrono::steady_clock::now();
    }

    std::unordered_map<std::string, Timer::TimerInfo> Timer::info;

    void Timer::Stop() {
        auto ending_time_point = std::chrono::steady_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(starting_time_point_);
        auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(ending_time_point);
        auto duration = end - start;
        if (info.find(function_name_) == info.end()) {
            TimerInfo time_info = {.time_sum = duration.count(), .number_of_calls = 1};
            info.insert({function_name_, time_info});
        } else {
            info[function_name_].number_of_calls++;
            info[function_name_].time_sum += duration.count();
        }
    }

    void Timer::Print() {
        for (const auto &pair : info) {
            auto fun_name = pair.first;
            auto time_info = pair.second;
            std::cout << "[Profiler] " << std::endl
                      << "- - Name : " << fun_name << std::endl
                      << "- - Avg : " << time_info.time_sum / time_info.number_of_calls << std::endl
                      << "- - Calls : " << time_info.number_of_calls << std::endl
                      << "- - Summed : " << time_info.time_sum << std::endl;
        }
    }

}