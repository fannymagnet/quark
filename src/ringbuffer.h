#pragma once

#include <atomic>

const uint32_t buffer_size = 1024*32;
const uint32_t cacheline_size = 64;
class RingBuffer
{
public:
	RingBuffer();
	~RingBuffer();

	uint32_t put(const uint8_t *data, uint32_t len);
	uint32_t get(uint8_t *data, uint32_t len);
	uint32_t add(uint32_t len);
	uint32_t erase(uint32_t len);

	inline uint32_t readable_bytes() {
		return in.load(std::memory_order_acquire) - out.load(std::memory_order_acquire);
	}

	inline uint32_t writeable_bytes() {
		return buffer_size - readable_bytes();
	}

	void get_writeable_buffer (struct iovec* vec, uint32_t& nv);
	void get_readable_buffer (struct iovec* vec, uint32_t& nv);

private:
	/* data */
	alignas(cacheline_size) std::atomic<unsigned int> in; //!< head index
	alignas(cacheline_size) std::atomic<unsigned int> out; //!< tail index
	uint32_t mask = buffer_size - 1;

	// put buffer after variables so everything can be reached with short offsets
	alignas(cacheline_size) uint8_t* buffer; //!< actual buffer
};