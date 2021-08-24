#pragma once

#include <chrono>
#include <memory>
#include <atomic>

namespace quark
{
	static uint64_t GetTimestamp()
	{
		const auto duration = std::chrono::steady_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	class TimerNode
	{
	public:
		TimerNode();
		virtual ~TimerNode();

		virtual void Start();
		virtual void Cancel();
		virtual void Run();
		bool IsActive();
		uint64_t GetTickTime();

	private:
		uint64_t tick_time_;
	};

	class Timer
	{
	public:
		Timer();
		~Timer();

		void Start();
		void Cancel();

	private:
		TimerNode *node_;
	};
} // namespace quark