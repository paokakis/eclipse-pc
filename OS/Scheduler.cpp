#include "Scheduler.h"
#include "../Engine/tools/Log.hpp"

Scheduler* Scheduler::instance;

void Scheduler::notifyTasks()
{
	std::vector<std::pair<int, Task*>>::iterator iter;
	for (iter = tasks.begin(); iter != tasks.end(); ++iter)
	{
		iter->second->notifyTask();
	}
}