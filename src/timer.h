#pragma once

#include <chrono>
#include <memory>
#include <atomic>

static uint64_t GetTimestamp() {
	const auto duration = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

class TimerNode {
public:
	TimerNode();
	~TimerNode();

	void Start();
	void Cancel();
	void Run();
	uint64_t GetTickTime();

private:
	std::atomic_bool flag_;
	uint64_t tick_time_;
	std::weak_ptr<Timer> timer_;	
};

class Timer: std::enable_shared_from_this<Timer> {
public:
	Timer();
	~Timer();

	void Start();
	void Cancel();
private:
	TimerNode* node_;
};