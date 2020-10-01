/*
 * Copyright 2020-present Open Networking Foundation
 * Copyright (c) 2019, Infosys Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef INCLUDE_CMN_CIRCULARFIFO_H_
#define INCLUDE_CMN_CIRCULARFIFO_H_

#include <atomic>
#include <cstddef>

namespace cmn {
	namespace utils {
		template<typename Element, size_t Size>
		class CircularFifo
		{
			public:
				enum { Capacity = Size + 1 };

				CircularFifo() : _tail(0), _head(0){}
				virtual ~CircularFifo() {}

				bool push(Element* item);
				bool pop(Element*& item);

				bool isLockFree() const;

			private:
				size_t increment(size_t idx) const;

				std::atomic <size_t>  _tail;
				Element*    _array[Capacity];
				std::atomic<size_t>   _head;
		};

		template<typename Element, size_t Size>
		bool CircularFifo<Element, Size>::push(Element* item)
		{
			const auto current_tail = _tail.load(std::memory_order_relaxed);
			const auto next_tail = increment(current_tail);
			if(next_tail != _head.load(std::memory_order_acquire))
			{
				_array[current_tail] = item;
				_tail.store(next_tail, std::memory_order_release);
				return true;
			}

			return false;
		}

		template<typename Element, size_t Size>
		bool CircularFifo<Element, Size>::pop(Element*& item)
		{
			const auto current_head = _head.load(std::memory_order_relaxed);
			if(current_head == _tail.load(std::memory_order_acquire))
				return false;

			item = _array[current_head];
			_head.store(increment(current_head), std::memory_order_release);
			return true;
		}

		template<typename Element, size_t Size>
		bool CircularFifo<Element, Size>::isLockFree() const
		{
			return (_tail.is_lock_free() && _head.is_lock_free());
		}

		template<typename Element, size_t Size>
		size_t CircularFifo<Element, Size>::increment(size_t idx) const
		{
			return (idx + 1) % Capacity;
		}
	}
}


#endif /* INCLUDE_CMN_CIRCULARFIFO_H_ */
