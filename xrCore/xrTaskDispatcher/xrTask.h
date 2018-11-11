#pragma once
#include "../xrDelegate/xrDelegate.h"

class xrTaskDispatcher;

enum TASK_PRIORITY
{
    TASK_PRIORITY_LOW = u8(0),    
    TASK_PRIORITY_HIGH,
};

enum TASK_STATE
{    
    STATE_WAIT,
    STATE_WORKING,
    STATE_READY
};

class xrTask
{
    friend class xrTaskDispatcher;
    friend class xrAsyncTaskDispatcher;

public:
    xrTask(TASK_PRIORITY priority) : d_taskPriority(priority) {}    
    xrTask(const xrTask& other) = delete;
    xrTask(xrTask&& other) noexcept = delete;
    xrTask& operator=(const xrTask& other) = delete;
    xrTask& operator=(xrTask&& other) noexcept = delete;
    virtual ~xrTask() = default;

    void wait() const
    {
        while (d_taskState != STATE_READY)
            Sleep(1);
    }

    bool ready() const
    {
        return d_taskState == STATE_READY;
    }

    bool waiting() const
    {
        return d_taskState == STATE_WAIT;
    }

    bool working() const
    {
        return d_taskState == STATE_WORKING;
    }

    TASK_PRIORITY getPriority() const
    {
        return d_taskPriority;
    }

    void setPriority(TASK_PRIORITY priority)
    {
        d_taskPriority = priority;
    }    

protected:
    virtual void invoke() = 0;
    volatile short d_taskState = STATE_WAIT;
    volatile bool d_taskCaptured = false;
    volatile TASK_PRIORITY d_taskPriority;    
};

template<typename Functor, typename TResult>
class xrTaskFunction : public xrTask
{
    using CallbackFunction = std::conditional_t<std::is_same_v<TResult, void>,
        std::function<void()>,
        std::function<void(TResult)>>;

    using TaskCallbackType = std::conditional_t<std::is_same_v<TResult, void>, int, TResult>;        

public:
    xrTaskFunction(Functor& functor, TASK_PRIORITY priority);

    template<typename ... Fx>
    void then(Fx&& ... callback);

protected:
    void invoke() override;

private:
    Functor d_functor;
    xrTaskDispatcher* d_dispatcher = nullptr;
    CallbackFunction d_callback;
    xrFastLock d_lock;
    TaskCallbackType d_result;
};

using xrTaskShared = std::shared_ptr<xrTask>;
