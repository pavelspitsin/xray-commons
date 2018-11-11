#pragma once
#include "xrDelegate.h"

namespace xrDelegateBinder
{
    template<typename T>
    struct DelegateFromFunctionTraits;

    template<typename V, typename R, typename ... Args>
    struct DelegateFromFunctionTraits<R(V::*)(Args...) const>
    {
        using DelegateType = xrDelegate<R(Args...)>;
        static constexpr size_t args_num = sizeof ... (Args);
    };

    template<typename V, typename R, typename ... Args>
    struct DelegateFromFunctionTraits<R(V::*)(Args...)>
    {
        using DelegateType = xrDelegate<R(Args...)>;
        static constexpr size_t args_num = sizeof ... (Args);
    };

    template<typename R, typename ... Args>
    struct DelegateFromFunctionTraits<R(*)(Args...)>
    {
        using DelegateType = xrDelegate<R(Args...)>;
        static constexpr size_t args_num = sizeof ... (Args);
    };

    template<typename T, typename Fx>
    struct DelegateFromFunction : DelegateFromFunctionTraits<Fx>
    {
        using DelegateType = typename DelegateFromFunctionTraits<Fx>::DelegateType;
        
        static auto Bind(T function)
        {
            return DelegateType(function);
        }

        template<typename V>
        static auto Bind(V ptr, T function)
        {
            return DelegateType(ptr, function);
        }

        static auto BindPtr(T function)
        {
            return new DelegateType(function);
        }

        template<typename V>
        static auto BindPtr(V ptr, T function)
        {
            return new DelegateType(ptr, function);
        }

    };

    enum DelegateType
    {
        DelegateDefault,        
        DelegatePtr
    };

    template<DelegateType type, typename T>
    static auto BindLambdaDelegate(T function)
    {
        using Tx = decltype(&T::operator());

        if constexpr (type == DelegateDefault)
            return DelegateFromFunction<T, Tx>::Bind(function);
        else
            return DelegateFromFunction<T, Tx>::BindPtr(function);        
    }

    template<DelegateType type, typename T>
    static auto BindFunctionDelegate(T function)
    {
        if constexpr (type == DelegateDefault)
            return DelegateFromFunction<T, T>::Bind(function);
        else
            return DelegateFromFunction<T, T>::BindPtr(function);        
    }

    template<DelegateType type, typename T, typename V>
    static auto BindFunctionDelegate(V ptr, T function)
    {
        if constexpr (type == DelegateDefault)
            return DelegateFromFunction<T, T>::Bind(ptr, function);
        else
            return DelegateFromFunction<T, T>::BindPtr(ptr, function);
    }
}

template<typename T>
static auto BindDelegate(T function)
{
    if constexpr (std::is_class_v<T>)
        return xrDelegateBinder::BindLambdaDelegate<xrDelegateBinder::DelegateDefault>(function);
    else
        return xrDelegateBinder::BindFunctionDelegate<xrDelegateBinder::DelegateDefault>(function);
}

template<typename V, typename T>
static auto BindDelegate(V ptr, T function)
{
    return xrDelegateBinder::BindFunctionDelegate<xrDelegateBinder::DelegateDefault>(ptr, function);
}

template<typename T>
static auto BindDelegatePtr(T function)
{
    if constexpr (std::is_class_v<T>)
        return xrDelegateBinder::BindLambdaDelegate<xrDelegateBinder::DelegatePtr>(function);
    else
        return xrDelegateBinder::BindFunctionDelegate<xrDelegateBinder::DelegatePtr>(function);
}

template<typename V, typename T>
static auto BindDelegatePtr(V ptr, T function)
{
    return xrDelegateBinder::BindFunctionDelegate<xrDelegateBinder::DelegatePtr>(function, ptr);
}

template<typename ... Args>
static xrDelegateArgumentsTypes<Args...> BindDelegateArgs(Args...args)
{
    return xrDelegateArgumentsTypes(std::forward<Args>(args)...);
}

template<typename ... Args>
static xrDelegateArgumentsTypes<Args...>* BindDelegateArgsPtr(Args&&...args)
{
    return new xrDelegateArgumentsTypes<Args...>(std::forward<Args>(args)...);
}