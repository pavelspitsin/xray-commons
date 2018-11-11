#pragma once
#include "BindFactoryRegistrator.h"
#include "BindFactoryProvider.h"

class BindFactory;

template<typename T>
class BindFactoryTypeRegistrator;

class BindFactoryRegistrator
{
public:
    template<typename T>
    BindFactoryTypeRegistrator<T>* implementation();
};

template<typename T>
class BindFactoryTypeRegistrator: public BindFactoryRegistrator
{
    friend class BindFactory;
    
    IBindFactoryProvider* d_bind = nullptr;
    BindFactory& d_factory;

protected:
    template<typename ... Args>
    T* get(Args&& ... args)
    {            
        return d_bind->implementation<T, Args...>()->get(std::forward<Args>(args)...);
    }

    template<typename ... Args>
    std::shared_ptr<T> getShared(Args&& ... args)
    {
        return d_bind->implementation<T, Args...>()->getShared(std::forward<Args>(args)...);
    }

public:
    BindFactoryTypeRegistrator(BindFactory& factory) : d_factory(factory) { }

    ~BindFactoryTypeRegistrator()
    {
        delete d_bind;
    }

    template<typename V, typename ... Args>
    void asTransient()
    {                
        d_bind = new BindFactoryTransientProvider<T, V, Args...>();
        injectInjectors();
    }

    template<typename V, typename ... Args>
    void asSingleton()
    {                        
        d_bind = new BindFactorySingletonProvider<T, V, Args...>();
        injectInjectors();
    }

    void asSingleton()
    {
        d_bind = new BindFactorySingletonProvider<T, T>();
        injectInjectors();
    }
    
    void asSingleton(T* instance)
    {        
        d_bind = new BindFactoryInstanceProvider<T>(instance);
        injectInjectors();
    }

    template<typename V, typename ... Args>
    void asThreadSingleton()
    {        
        d_bind = new BindFactoryThreadSingletonProvider<T, V, Args...>();
        injectInjectors();
    }

    template<typename V, typename ... Args>
    void asProvider(BindFactoryProvider<V, Args...>* provider)
    {
        d_bind = provider;
        injectInjectors();
    }

private:
    void injectInjectors() const;
};

template<typename T>
BindFactoryTypeRegistrator<T>* BindFactoryRegistrator::implementation()
{
    return static_cast<BindFactoryTypeRegistrator<T>*>(const_cast<BindFactoryRegistrator*>(this));
}
