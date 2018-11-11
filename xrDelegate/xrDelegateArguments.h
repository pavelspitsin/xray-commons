#pragma once

template<typename ... Args>
class xrDelegateArgumentsTypes;

class xrDelegateArguments
{
public:
    virtual ~xrDelegateArguments() = default;
    
    template<typename ... Args>
    xrDelegateArgumentsTypes<Args...>& get()
    {
        xrDelegateArguments* self = const_cast<xrDelegateArguments*>(this);
        return static_cast<xrDelegateArgumentsTypes<Args...>&>(*self);
    }
};

template<typename ... Args>
class xrDelegateArgumentsTypes : public xrDelegateArguments
{
public:
    using tuple_type = std::tuple<Args...>;

    xrDelegateArgumentsTypes(Args&& ... args) : d_args(args...) {}
    
    tuple_type& values() { return d_args; }

    auto&& first() { return std::get<0>(d_args); }

private:
    tuple_type d_args;
};