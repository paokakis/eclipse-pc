#ifndef TASK_H
#define TASK_H
#include <thread>
#include <mutex>
#include <condition_variable>

static bool syncBool = false;

class Task
{
	friend class Scheduler;
public:
	Task()
	{
		mTaskRun = false;
		alive = true;
		mSleepTime = 1.0;
		sleepable = true;
	}

	virtual ~Task() 
	{
		if (mTaskRun)
		{
			taskStop();
		}
	}
protected:	
	void setDelayuSec(double del) 
	{ 
		mMutex->lock(); 
		mSleepTime = del; 
		mMutex->unlock(); 
	}
	virtual void taskRun() {}
	std::mutex* mMutex = NULL;
	volatile bool mTaskRun;
	volatile bool sleepable; // = true;
private:
	std::thread* mTaskThread = NULL;
	volatile bool alive;
	void taskCb();	
	volatile double mSleepTime;
private:
	void taskStart() 
	{
		if (!mTaskRun)
		{
			mTaskThread = new std::thread(&Task::taskCb, this);
			mMutex = new std::mutex();
			mTaskRun = true;
		}
	}

	void taskStop()
	{
		if (mTaskRun)
		{
			mTaskRun = false;
			alive = false;
			mTaskThread->join();
		}
	}
	void notifyTask()
	{
		mMutex->lock();
		mTaskRun = true;
		mMutex->unlock();
	}
};

#endif // TASK_H