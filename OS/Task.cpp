#include "Task.h"
#include <condition_variable>
#include "../Engine/tools/Log.hpp"

void Task::taskCb()
{
	while (alive)
	{
		if (mTaskRun)
		{
			mMutex->lock();
			this->taskRun();
			if (sleepable) mTaskRun = false;
			mMutex->unlock();
		}
		else
		{
			do {
				std::this_thread::yield();
			} while (!mTaskRun);
		}
	}
}
