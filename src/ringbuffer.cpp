#include "ringbuffer.h"
//#include "easylogging++.h"
#include <assert.h>
#include <limits>
#include <string.h>

#include <sys/uio.h>

namespace quark
{
	uint32_t min(uint32_t a, uint32_t b)
	{
		return a > b ? b : a;
	}

	RingBuffer::RingBuffer() : in(0), out(0)
	{
		buffer = new uint8_t[buffer_size]{1};
	}

	RingBuffer::~RingBuffer()
	{
		if (nullptr != buffer)
		{
			delete[] buffer;
			buffer = nullptr;
		}
	}

	// 返回实际写入缓冲区中的数据
	uint32_t RingBuffer::put(const uint8_t *data, uint32_t len)
	{
		// 当前缓冲区空闲空间
		len = min(len, writeable_bytes());

		// 当前in位置到buffer末尾的长度
		auto l = min(len, buffer_size - (in & mask));

		// 首先复制数据到[in，buffer的末尾]
		memcpy(buffer + (in & mask), data, l);

		// 复制剩余的数据（如果有）到[buffer的起始位置,...]
		memcpy(buffer, data + l, len - l);

		in += len; // 直接加，不作模运算。当溢出时，从buffer的开始位置重新开始

		return len;
	}

	// 返回实际读取的数据长度
	uint32_t RingBuffer::get(uint8_t *data, uint32_t len)
	{
		// 缓冲区中的数据长度
		len = min(len, in - out);

		// 首先从[out,buffer end]读取数据
		auto l = min(len, buffer_size - (out & mask));
		memcpy(data, buffer + (out & mask), l);

		// 从[buffer start,...]读取数据
		memcpy(data + l, buffer, len - l);

		out += len; // 直接加，不错模运算。溢出后，从buffer的起始位置重新开始

		return len;
	}

	// 返回实际写入的数据长度
	uint32_t RingBuffer::add(uint32_t len)
	{
		// 缓冲区中的数据长度
		len = min(len, writeable_bytes());
		in += len; // 直接加，不错模运算。溢出后，从buffer的起始位置重新开始
		// //LOG(INFO) << "==== "
		// 		  << "ringbuffer write : " << len << " current: " << in;

		return len;
	}
	// 返回实际擦除的数据长度
	uint32_t RingBuffer::erase(uint32_t len)
	{
		// 缓冲区中的数据长度
		len = min(len, in - out);
		out += len; // 直接加，不错模运算。溢出后，从buffer的起始位置重新开始

		// //LOG(INFO) << "==== "
		// 		  << "ringbuffer read : " << len << " current: " << out;
		return len;
	}

	void RingBuffer::get_writeable_buffer(MultiIoBuf& buf)
	{
		//assert(nv == 2);
		// 到缓冲区尾部
		auto size = writeable_bytes();
		auto l = buffer_size - (in & mask);
		auto len = min(l, size);
		buf.Push((buffer + (in & mask)), len);

		auto left = writeable_bytes() - len;
		if (left > 0)
		{
			buf.Push(buffer, left);
		}

		/*
	//LOG(INFO) << "--- "
			  << "ringbuffer write iovec: " << nv << " left: " << left;
	//LOG(INFO) << "--- " << vec[0].iov_base << " : " << vec[0].iov_len;
	//LOG(INFO) << "--- " << vec[1].iov_base << " : " << vec[1].iov_len;
	*/
	}

	void RingBuffer::get_readable_buffer(MultiIoBuf& buf)
	{
		auto size = readable_bytes();
		// 到缓冲区尾部
		auto len = buffer_size - (out & mask);
		len = min(len, size);
		buf.Push((buffer + (out & mask)), len);

		auto left = size - len;
		if (left > 0)
		{
			buf.Push(buffer, left);
		}

		/*
	//LOG(INFO) << "--- "
			  << "ringbuffer read iovec: " << nv << " writeable: " << size;
	//LOG(INFO) << "--- " << vec[0].iov_base << " : " << vec[0].iov_len;
	//LOG(INFO) << "--- " << vec[1].iov_base << " : " << vec[1].iov_len;
	*/
	}
}