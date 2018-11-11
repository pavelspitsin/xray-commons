#pragma once
#include "xrTask.h"

template <typename Functor, typename TResult>
xrTaskFunction<Functor, TResult>::xrTaskFunction(Functor& functor, TASK_PRIORITY priority)
    : xrTask(priority),
    d_functor(functor),
    d_dispatcher(xrTaskDispatcher::getCurrentThreadDispatcher())
{
}

template <typename Functor, typename TResult>
template <typename ... Fx>
void xrTaskFunction<Functor, TResult>::then(Fx&&... callback)
{
    ScopedLock(d_lock);    
    R_ASSERT(d_callback == nullptr);

    if constexpr (std::is_same_v<TResult, void>)
    {
        d_callback = std::bind(std::forward<Fx>(callback) ...);
        
        if (ready())
            d_callback();
    }
    else
    {
        d_callback = std::bind(std::forward<Fx>(callback) ..., std::placeholders::_1);
        
        if (ready())
            d_callback(d_result);
    }
}

template <typename Functor, typename TResult>
void xrTaskFunction<Functor, TResult>::invoke()
{    
    d_taskState = STATE_WORKING;

    if constexpr (std::is_same_v<TResult, void>)
    {
        d_functor();
            
        if (d_callback)
        {
			ScopedLock(d_lock);
			d_dispatcher->addTask(d_callback);
        }
    }
    else
    {
        d_result = d_functor();
              
        if (d_callback)
        {
			ScopedLock(d_lock);
			d_dispatcher->addTask(d_callback, d_result);
        }                
    }

    d_taskState = STATE_READY;
}
