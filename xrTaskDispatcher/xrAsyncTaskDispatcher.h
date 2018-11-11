#pragma once
#include "xrTask.h"
#include <thread>
#include "../xrArrayHelpers.h"
#include "xrTaskDispatcher.h"

using AsyncTaskSharedQueue = std::vector<xrTaskShared>;

class xrAsyncTaskDispatcher
{    
public:
    xrAsyncTaskDispatcher()
    {        
        for (size_t pos = 0; pos < CPU::xrSystemInfo.dwNumberOfProcessors; pos++)
            d_threads.push_back(new std::thread(&xrAsyncTaskDispatcher::threadDispatcher, this, pos));
    }

    ~xrAsyncTaskDispatcher()
    {
        d_terminate = true;

        while (d_terminate)
            Sleep(0);        

        for (auto thread : d_threads)
            delete thread;
    }

    xrAsyncTaskDispatcher(const xrAsyncTaskDispatcher& other) = delete;
    xrAsyncTaskDispatcher(xrAsyncTaskDispatcher&& other) noexcept = delete;
    xrAsyncTaskDispatcher& operator=(const xrAsyncTaskDispatcher& other) = delete;
    xrAsyncTaskDispatcher& operator=(xrAsyncTaskDispatcher&& other) noexcept = delete;

    template<TASK_PRIORITY Priority = TASK_PRIORITY_LOW, typename ... Fx>
    static auto addTask(Fx ... args)
    {
        auto functor = std::bind(std::forward<Fx>(args)...);
        using TFunctor = decltype(functor);
        using TResult = typename std::remove_reference<typename std::result_of<TFunctor()>::type>::type;
        using Task = xrTaskFunction<TFunctor, TResult>;
        std::shared_ptr<Task> task = std::make_shared<Task>(functor, Priority);
		g_async_task_dispatcher.get()->push(task);
        return task;
    }
    
    template<TASK_PRIORITY Priority = TASK_PRIORITY_LOW, typename ... Fx>
    static auto addTaskToQueue(AsyncTaskSharedQueue& queue, Fx ... args)
    {
        auto task = addTask(args...);        
        queue.push_back(task);
        return task;
    }
    
    static void waitQueue(AsyncTaskSharedQueue& queue)
    {
        for (auto& item : queue)
        {
            item->wait();
        }
		queue.clear();
    }

private:
    void push(const xrTaskShared& task)
    {     
        auto dispatcher = d_thread_dispatchers.at(0);

	    if (dispatcher->totalQueued() != 0) //Пустая очередь нас и так устроит
	    {
		    for (auto& entry : d_thread_dispatchers)
		    {
			    if (entry.second->totalQueued() < dispatcher->totalQueued())
				    dispatcher = entry.second;
		    }
	    }

        dispatcher->push(task);
    }

    void threadDispatcher(size_t id)
    {
        shared_str temp_str;
        temp_str.printf("xrAsyncTaskDispatcherThread #%d", id);
        thread_name(temp_str.c_str());
        _initialize_cpu_thread();

		auto dispatcher = new xrTaskDispatcher();
        {
	        ScopedLock(d_locker);
			d_thread_dispatchers[id] = dispatcher;
        }
        while (!d_terminate)
        {          
            dispatcher->dispatch();

			if (dispatcher->totalQueued() == 0)
				Sleep(1);
        }

        d_terminate = false;
    }
        
    std::map<size_t, xrTaskDispatcher*> d_thread_dispatchers;    
    std::vector<std::thread*> d_threads;    
    volatile bool d_terminate = false;
    xrFastLock d_locker {};
	IC static xrInject<xrAsyncTaskDispatcher>	g_async_task_dispatcher;
};

