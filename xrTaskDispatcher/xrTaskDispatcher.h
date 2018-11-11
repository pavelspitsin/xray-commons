#pragma once
#include <thread>
#include "xrTask.h"

constexpr bool g_disableTaskPriority = true;

class XRCORE_API xrTaskDispatcher
{
    friend class xrAsyncTaskDispatcher;

public:
    xrTaskDispatcher() 
    {
        d_thread_id = std::this_thread::get_id();
        d_dispatchersMap[d_thread_id] = this;
    }

    virtual ~xrTaskDispatcher() 
    {
        d_dispatchersMap[d_thread_id] = nullptr;
    }

    xrTaskDispatcher(const xrTaskDispatcher& other) = delete;
    xrTaskDispatcher(xrTaskDispatcher&& other) = delete;
    xrTaskDispatcher& operator=(const xrTaskDispatcher& other) = delete;
    xrTaskDispatcher& operator=(xrTaskDispatcher&& other) = delete;    
    
    template<TASK_PRIORITY Priority = TASK_PRIORITY_LOW, typename ... Fx>
    auto addTask(Fx ... args)
    {   
        auto functor = std::bind(std::forward<Fx>(args)...);
        using TFunctor = decltype(functor);
        using TResult = typename std::remove_reference<typename std::result_of<TFunctor()>::type>::type;
        using Task = xrTaskFunction<TFunctor, TResult>;
        std::shared_ptr<Task> task = std::make_shared<Task>(functor, Priority);
        push(task);
        return task;
    }

    void dispatch()
    {
        auto callerThread = std::this_thread::get_id();
        R_ASSERT(callerThread == d_thread_id);        

        d_lock.Enter();
        d_queued = std::move(d_queue);        
        d_lock.Leave();

        d_queue_size = d_queued.size();

        while (!d_queued.empty())
        {            
            d_queued.front()->invoke();
            d_queued.pop_front();
            --d_queue_size;
        }     
    }

    static xrTaskDispatcher* getCurrentThreadDispatcher()
    {
        auto thread = std::this_thread::get_id();
        return d_dispatchersMap[thread];
    }

    static xrTaskDispatcher* getThreadDispatcher(const std::thread::id& thread_id)
    {        
        return d_dispatchersMap[thread_id];
    }

private:
    void push(const xrTaskShared& task)
    {
        ScopedLock(d_lock);

        if (task->getPriority() == TASK_PRIORITY_LOW)
            d_queue.push_back(task);
        else
            d_queue.push_front(task);

        d_queue_size++;
    }

    size_t totalQueued() const
    {
        return d_queue_size;
    }

    using DispatcherThreadMap = std::map<std::thread::id, xrTaskDispatcher*>;        
    static DispatcherThreadMap d_dispatchersMap;

    using SharedTaskQueue = std::deque<std::shared_ptr<xrTask>>;
    
    // Tasks that will be dispatched
    SharedTaskQueue d_queue;

    // Tasks that will aready dispatched
    SharedTaskQueue d_queued;
           
    xrFastLock d_lock;
    std::thread::id d_thread_id;

    size_t d_queue_size = 0;
};

#include "xrTask_inline.h"