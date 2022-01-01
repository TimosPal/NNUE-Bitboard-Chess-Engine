#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <unordered_map>

#ifdef PROFILER
#define PROFILE_SCOPE(name) ChessEngine::Timer timer##__LINE__(name)
#else
#define PROFILE_SCOPE(name)
#endif
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)

namespace ChessEngine {

    class Timer{
    public:
        struct TimerInfo{
            long long int time_sum = 0;
            int number_of_calls = 0;
        };

        explicit Timer(std::string function_name);
        ~Timer(){ Stop(); }
        void Stop();
        static void Print();
    private:
        std::chrono::time_point<std::chrono::steady_clock> starting_time_point_;
        std::string function_name_;

        static std::unordered_map<std::string, TimerInfo> info;
    };

};


#endif
