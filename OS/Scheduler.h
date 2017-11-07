#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "Task.h"
#include <vector>
#include <condition_variable>

class Scheduler {
public:
	static Scheduler* getInstance() 
	{
		if (instance == NULL)
		{
			instance = new Scheduler();
		}
		return instance;
	}

	void registerTask(Task* task, int priority)
	{
		tasks.push_back(std::make_pair(priority, task));
	}

	void start() 
	{
		if (!running)
		{
			std::vector<std::pair<int, Task*>>::iterator iter;
			for (iter = tasks.begin(); iter != tasks.end(); ++iter)
			{
				iter->second->taskStart();
			}
		}
	}

	void startTask(Task* pTask)
	{
		if (!running)
		{
			std::vector<std::pair<int, Task*>>::iterator iter;
			for (iter = tasks.begin(); iter != tasks.end(); ++iter)
			{
				if (iter->second == pTask) iter->second->taskStart();
			}
		}
	}

	void stop()
	{
		if (running)
		{
			std::vector<std::pair<int, Task*>>::iterator iter;
			for (iter = tasks.begin(); iter != tasks.end(); ++iter)
			{
				iter->second->alive = false;
				iter->second->taskStop();
			}
		}
	}

	void stopTask(Task* pTask)
	{
		if (!running)
		{
			std::vector<std::pair<int, Task*>>::iterator iter;
			for (iter = tasks.begin(); iter != tasks.end(); ++iter)
			{
				if (iter->second == pTask) iter->second->taskStop();
			}
		}
	}

	void notifyTasks();
private:
	explicit Scheduler() {}
	~Scheduler() {}
	static Scheduler* instance;

	std::vector<std::pair<int, Task*>> tasks;
	bool running = false;
};


#endif // SCHEDULER_H