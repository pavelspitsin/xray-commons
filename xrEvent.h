#pragma once
#include "xrDelegate/xrDelegate.h"
#include "xrArrayHelpers.h"

template<typename ... Args>
class xrEvent
{
public:
    xrEvent() = default;
    xrEvent(const xrEvent& other) = delete;
    xrEvent(xrEvent&& other) = delete;
    xrEvent& operator=(const xrEvent& other) = delete;
    xrEvent& operator=(xrEvent&& other) = delete;
    xrEvent operator*() = delete;
    
    template<typename TFunction>
    void subscribe(TFunction fx) const
    {        
        d_subscribers.push_back(BindDelegate(fx));        
    }

    template<typename TClass, typename TFunction>
    void subscribe(TClass tx, TFunction fx) const
    {
        d_subscribers.push_back(BindDelegate(tx, fx));
    }

    template<typename TFunction>
    void unsubscribe(TFunction fx) const
    {
        auto self = BindDelegate(fx);
        array_eraseif(d_subscribers, [self](const xrDelegate<void(Args...)>& delegate)
        {
            return self == delegate;
        });
    }

    template<typename TClass, typename TFunction>
    void unsubscribe(TClass tx, TFunction fx) const
    {
        auto self = BindDelegate(tx, fx);
        array_eraseif(d_subscribers, [self](const xrDelegate<void(Args...)>& delegate)
        {
            return self == delegate;
        });
    }  

    void operator()(Args...args)
    {
        for (auto it : d_subscribers)
            it(args...);
    }

private:
    mutable std::vector<xrDelegate<void(Args...)>> d_subscribers;
};
