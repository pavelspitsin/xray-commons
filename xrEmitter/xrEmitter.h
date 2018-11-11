#pragma once
#include <map>
#include <list>
#include "../xrDelegate/xrDelegate.h"
#include "../xrArrayHelpers.h"

template<typename Key>
class xrEmitterCore
{    
public:
    virtual ~xrEmitterCore() = default;

    template<typename T>
    void subscribe(const Key& event, T function)
    {        
        push_impl(event, BindDelegatePtr(function));
    }

    template<typename T, typename V>
    void subscribe(const Key& event, T function, V ptr)
    {
        push_impl(event, BindDelegatePtr(function, ptr));
    }

    template<typename T>
    void unsubscribe(const Key& event, T function)
    {
        pop_impl(event, BindDelegatePtr(function));
    }

    template<typename T, typename V>
    void unsubscribe(const Key& event, T function, V ptr)
    {
        pop_impl(event, BindDelegatePtr(function, ptr));
    }

    template<typename ... Args>
    void emit(const Key& event, Args ... args)
    {
        if constexpr (sizeof ... (Args) > 0)
            emit_impl(event, BindDelegateArgsPtr(std::forward<Args>(args)...));
        else
            emit_impl(event, nullptr);
    }

protected:
    virtual void emit_impl(const Key& event, xrDelegateArguments* args) = 0;
    virtual void push_impl(const Key& event, xrAbstractDelegate<void>* handler) = 0;
    virtual void pop_impl(const Key& event, xrAbstractDelegate<void>* handler) = 0;
};

template<typename Key, typename KeyComparator>
class xrSharedEmitterType;

template<typename Key, typename KeyComparator = std::less<Key>>
class xrEmitterType : public xrEmitterCore<Key>
{    
    friend class xrSharedEmitterType<Key, KeyComparator>;

public:    
    virtual ~xrEmitterType()
    {
        auto map_it = d_map.begin();
        auto map_ite = d_map.end();
        while (map_it != map_ite)
        {
            auto sub_it = map_it->second.begin();
            auto sub_ite = map_it->second.end();
            while (sub_it != sub_ite)
                delete (*sub_it++);
            ++map_it;
        }
    }

protected:
    using SubscribersList = std::list<xrAbstractDelegate<void>*>;
    using SubscribersMap = std::map<Key, SubscribersList, KeyComparator>;
    SubscribersMap d_map;

    virtual void emit_impl(const Key& event, xrDelegateArguments* args)
    {
        auto& d_subscribers = d_map[event];
        auto it = d_subscribers.begin();
        auto ite = d_subscribers.end();
        while (it != ite)
            (*it++)->invoke_args(*args);

        delete args;
    }

    virtual void push_impl(const Key& event, xrAbstractDelegate<void>* handler)
    {
        d_map[event].push_back(handler);        
    }

    virtual void pop_impl(const Key& event, xrAbstractDelegate<void>* handler)
    {
        auto& d_subscribers = d_map[event];

        auto result = std::find_if(d_subscribers.begin(), d_subscribers.end(), [handler](xrAbstractDelegate<void>* callback)
        {
            return *callback == *handler;
        });

        if (result != d_subscribers.end())
        {
            delete handler;
            delete *result;
            d_subscribers.erase(result);
        }
    }
};

#pragma warning(push)
#pragma warning(disable:4995)
struct CharCompare
{
    bool operator()(const char* a, const char* b) const
    {
        return std::strcmp(a, b) < 0;
    }
};
#pragma warning(pop)

using xrEmitter = xrEmitterType<const char*, CharCompare>;