#pragma once

template<typename T, typename ... Args>
class BindFactoryProvider;

class IBindFactoryProvider
{
public:
    virtual ~IBindFactoryProvider() = default;
    
    template<typename T, typename ... Args>
    BindFactoryProvider<T, Args...>* implementation();
};

template<typename T, typename ... Args>
class BindFactoryProvider : public IBindFactoryProvider
{
public:
    BindFactoryProvider() = default;
    virtual ~BindFactoryProvider() = default;
    virtual T* get(Args&& ... args) = 0;
    virtual std::shared_ptr<T> getShared(Args&& ... args) = 0;
};

template <typename T, typename ... Args>
BindFactoryProvider<T, Args...>* IBindFactoryProvider::implementation()
{
    return static_cast<BindFactoryProvider<T, Args...>*>(const_cast<IBindFactoryProvider*>(this));
}

template<typename T, typename V, typename ... Args>
class BindFactorySingletonProvider : public BindFactoryProvider<T, Args ...>
{
public:
    BindFactorySingletonProvider() = default;

    ~BindFactorySingletonProvider()
    {
        delete d_singleton;
    }

    T* get(Args&& ... args) override
    {
        if (!d_singleton)
            d_singleton = new V(std::forward<Args>(args) ...);

        return d_singleton;
    }

    std::shared_ptr<T> getShared(Args&& ... args) override
    {
        return std::shared_ptr<T>(get(std::forward<Args>(args)...), destroy);
    }

    static void destroy(T* ptr) {}

private:
    T* d_singleton = nullptr;
};

template<typename T, typename ... Args>
class BindFactoryInstanceProvider : public BindFactoryProvider<T, Args ...>
{
public:
    BindFactoryInstanceProvider(T* instance) 
        : d_singleton(instance) { }

    T* get(Args&& ... args) override
    {
        return d_singleton;
    }

    std::shared_ptr<T> getShared(Args&& ... args) override
    {
        return std::shared_ptr<T>(d_singleton, destroy);
    }

    static void destroy(T* ptr) {}

private:
    T* d_singleton = nullptr;
};

template<typename T, typename V, typename ... Args>
class BindFactoryThreadSingletonProvider : public BindFactoryProvider<T, Args ...>
{
public:
    T* get(Args&& ... args) override
    {
        thread_local V d_object(std::forward<Args>(args)...);
        return &d_object;
    }

    std::shared_ptr<T> getShared(Args&& ... args) override
    {
        return std::shared_ptr<T>(get(std::forward<Args>(args)...), destroy);
    }

    static void destroy(T* ptr) {}
};

template<typename T, typename V, typename ... Args>
class BindFactoryTransientProvider : public BindFactoryProvider<T, Args ...>
{
public:
    T* get(Args&& ... args) override
    {
        return new V(args ...);
    }

    std::shared_ptr<T> getShared(Args&& ... args) override
    {
        return std::shared_ptr<T>(get(std::forward<Args>(args)...), destroy);
    }

    static void destroy(T* ptr)
    {
        delete static_cast<V*>(ptr);
    }
};
