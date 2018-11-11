#pragma once
#include "xrEmitter.h"
#include <thread>

template<typename Key, typename KeyComparator = std::less<Key>>
class xrSharedEmitterType : public xrEmitterType<Key>
{
public:
    xrSharedEmitterType() : d_thread_id(std::this_thread::get_id()) {}

    void dispatch()
    {
        while (!d_internal_event_queue.empty())
        {
            auto& e = d_internal_event_queue.front();
            d_current_emitter.emit_impl(e->d_event, e->d_args);
            d_internal_event_queue.pop_front();
        }
    }

    void clearEventsQueue()
    {
        auto thread = std::this_thread::get_id();
        if (thread == d_thread_id)
            d_internal_event_queue.clear();
        else
            d_current_event_queue.clear();
    }

protected:
    void emit_impl(const Key& event, xrDelegateArguments* args) override
    {
        auto thread = std::this_thread::get_id();
        if (thread == d_thread_id)
            d_internal_event_queue.emplace_back(std::make_shared<Event>(event, args));
        else
            d_current_event_queue.emplace_back(std::make_shared<Event>(event, args));
    }

    void push_impl(const Key& event, xrAbstractDelegate<void>* handler) override
    {
        auto thread = std::this_thread::get_id();

        if (thread == d_thread_id)
            d_current_emitter.d_map[event].push_back(handler);
        else
            d_internal_emitter.d_map[event].push_back(handler);
    }

    void pop_impl(const Key& event, xrAbstractDelegate<void>* handler) override
    {
        auto thread = std::this_thread::get_id();
        if (thread == d_thread_id)
            d_current_emitter.pop_impl(event, handler);
        else
            d_internal_emitter.pop_impl(event, handler);
    }

    struct Event
    {
        Event(const Key& event, xrDelegateArguments* xrDelegateArguments)
            : d_event(event), d_args(xrDelegateArguments) {}

        Key d_event;
        xrDelegateArguments* d_args = nullptr;
    };

    std::deque<std::shared_ptr<Event>> d_current_event_queue;
    std::deque<std::shared_ptr<Event>> d_internal_event_queue;

    xrEmitterType<Key, KeyComparator> d_current_emitter;
    xrEmitterType<Key, KeyComparator> d_internal_emitter;

    std::thread::id d_thread_id;
};

using xrSharedEmitter = xrSharedEmitterType<const char*, CharCompare>;