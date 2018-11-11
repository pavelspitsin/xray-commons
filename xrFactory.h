#pragma once
#include "BindFactory/BindFactory.h"

class XRCORE_API xrFactory
{
protected:
    xrFactory() = default;
    ~xrFactory() = default;

public:
    template<typename T>
    static BindFactoryTypeRegistrator<T>* bind()
    {
        return d_factory.bind<T>();
    }

    template<typename T, typename ... Args>
    static T* get(Args ... args)
    {
        return d_factory.get<T>(args...);
    }

    template<typename T, typename V, typename ... Args>
    static V* get(Args ... args)
    {
        return d_factory.get<T, V>(args ...);
    }

    template<typename T, typename ... Args>
    static std::shared_ptr<T> getShared(Args ... args)
    {
        return d_factory.getShared<T>(args ...);
    }

    template<typename T, typename V, typename ... Args>
    static std::shared_ptr<V> getShared(Args ... args)
    {
        return d_factory.getShared<T, V>(args ...);
    }

    template<typename T>
    static void unbind()
    {
        return d_factory.unbind<T>();
    }

    static void bind(BindFactory::Injector* inject)
    {
        return d_factory.bind(inject);
    }
    
    static void unbind(BindFactory::Injector* inject)
    {
        return d_factory.unbind(inject);
    }

private:
    static BindFactory d_factory;
};

template<typename T, typename ... Args>
class xrInject : public BindFactoryTypeInjector<T>
{
public:
    xrInject(Args ... args) : d_args(args...)
    {
        xrFactory::bind(this);
    }

    ~xrInject()
    {
        xrFactory::unbind(this);
    }

    void inject() override
    {
        inject_type(d_args, std::make_index_sequence<std::tuple_size<typename std::decay<std::tuple<Args...>>::type>::value>{});
    }

    void uninject() override
    {
        d_object = nullptr;
    }

private:
    template<typename TupleType, std::size_t... index>
    void inject_type(TupleType& tup, std::index_sequence<index...>) {        
        d_object = xrFactory::get<T>(std::get<index>(tup)...);
    }

    std::tuple<Args...> d_args;
};

template<typename T, typename ... Args>
class xrSharedInject : public BindFactoryTypeSharedInjector<T>
{
public:
    xrSharedInject(Args ... args) : d_args(args...)
    {
        xrFactory::bind(this);
    }

    ~xrSharedInject()
    {
        xrFactory::unbind(this);
    }

    void inject() override
    {
        inject_type(d_args, std::make_index_sequence<std::tuple_size<typename std::decay<std::tuple<Args...>>::type>::value>{});
    }

    void uninject() override
    {
        d_object = nullptr;
    }

private:
    template<typename TupleType, std::size_t... index>
    void inject_type(TupleType& tup, std::index_sequence<index...>) {
        d_object = xrFactory::getShared<T>(std::get<index>(tup)...);
    }

    std::tuple<Args...> d_args;
};