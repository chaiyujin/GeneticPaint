/* -----------------------------
* File   : timer.h
* Author : Yuki Chai
* Created: 2017.03.31
* Project: Yuki
*/
#pragma once
#ifndef __YUKI_UTILS_TIMER_H__
#define __YUKI_UTILS_TIMER_H__

#include <chrono>
#include <string>
#include <iostream>

namespace Yuki {

	enum TimeType {
		MS = 0,
		SEC = 1
	};

	class Timer {
	public:
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = std::chrono::time_point<Clock>;

		Timer() {
			restart();
		}

		// restart the timer
		void restart() {
			start_time_ = Clock::now();
		}

		// return duration in seconds
		double duration() const {
			auto end_time = Clock::now();
			auto m = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_).count();
			return m * 1.0 / 1e6;
		}

	protected:
		TimePoint start_time_;
	};

	class StopWatch: public Timer {
	public:

		StopWatch(std::string event_name = "", bool print_log = true, TimeType time_type = TimeType::SEC)
			: event_name(event_name), time_type(time_type), stopped(false), log(print_log) {
			this->event_name = "[" + event_name + "]";
			if (log) {
				std::cout << "#--------#\n";
				std::cout << "StopWatch: " << this->event_name << " begins." << std::endl;
			}
		}

		~StopWatch() {
			if (!stopped) stop();
		}

		double stop() {
			if (log && event_name.length()) {
				std::cout << "StopWatch: " << event_name << " costs ";
			}

			double dura = duration();

			switch (time_type) {
			case TimeType::MS:
				dura *= 1000.;
				if (log) {
					std::cout << std::to_string(dura) << " milliseconds." << std::endl;
					std::cout << "\\--------/\n";
				}
				break;
			case TimeType::SEC:
				if (log) {
					std::cout << std::to_string(dura) << " seconds." << std::endl;
					std::cout << "\\--------/\n";
				}
				break;
			}

			stopped = true;

			return dura;
		}

		void open_log() { log = true; }
		void close_log() { log = false; }

	protected:
		std::string event_name;
		TimeType time_type;
		bool stopped;
		bool log;
	};

}

#endif  // !__YUKI_UTILS_TIMER_H__