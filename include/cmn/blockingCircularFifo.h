 /*
 * Copyright 2019-present Infosys Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_CMN_BLOCKINGCIRCULARFIFO_H_
#define INCLUDE_CMN_BLOCKINGCIRCULARFIFO_H_

#include <cstddef>
#include <semaphore.h>
#include "circularFifo.h"

namespace cmn {
	namespace utils {

		template<typename Element, size_t size>
		class BlockingCircularFifo
		{
		public:
			BlockingCircularFifo():circularQueue()
			{
				sem_init(&pop_semaphore, 0, 0);
				sem_init(&push_semaphore, 0, size);
			}

			~BlockingCircularFifo()
			{
				sem_destroy(&pop_semaphore);
				sem_destroy(&push_semaphore);
			}

			bool push(Element* item)
			{
				sem_wait(&push_semaphore);
				bool ret = circularQueue.push(item);
				if (ret)
					sem_post(&pop_semaphore);
				else
					sem_post(&push_semaphore);
				return ret;
			}

			bool pop(Element*& item)
			{
				sem_wait(&pop_semaphore);
				bool ret = circularQueue.pop(item);
				if (ret)
					sem_post(&push_semaphore);
				else
					sem_post(&pop_semaphore);
				return ret;
			}

		private:
			CircularFifo<Element, size> circularQueue;
			sem_t pop_semaphore;
			sem_t push_semaphore;
		};
	}
}
#endif /* INCLUDE_CMN_BLOCKINGCIRCULARFIFO_H_ */
