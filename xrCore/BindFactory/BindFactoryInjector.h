#pragma once

/**
 * \brief Provides a basic interface for injector.
 */
class BindFactoryInjector
{
public:
    virtual ~BindFactoryInjector() = default;
    virtual void inject() = 0;
    virtual void uninject() = 0;
    virtual size_t hash() = 0;
};

/**
 * \brief Provides a basic methods and operators for type injection.
 * \tparam Type
 */
template<typename Type>
class BindFactoryTypeInjector : public BindFactoryInjector
{    
protected:
    BindFactoryTypeInjector() = default;

public:
    BindFactoryTypeInjector(const BindFactoryTypeInjector& other) = delete;
    BindFactoryTypeInjector(BindFactoryTypeInjector&& other) noexcept = delete;
    BindFactoryTypeInjector& operator=(const BindFactoryTypeInjector& other) = delete;
    BindFactoryTypeInjector& operator=(BindFactoryTypeInjector&& other) noexcept = delete;
    virtual ~BindFactoryTypeInjector() = default;

    /**
     * \brief Returns a pointer of injected instance.
     */
    Type* operator->() const
    {
        return d_object;
    }

    /**
     * \brief Returns a reference of injected instance.
     */
    Type& operator*() const
    {
        return *d_object;
    }

    /**
     * \brief Checks if pointer of injected instance not null.
     */
    operator bool() const
    {
        return d_object != nullptr;
    }

    /**
     * \brief Checks if pointer of injected instance is null.
     */
    bool operator! () const
    {
        return d_object == nullptr;
    }

    /**
     * \brief Returns a pointer of injected instance.
     */
    Type* get() const
    {
        return d_object;
    }

    /**
     * \brief Returns hash of injected.
     */
    size_t hash() override
    {
        return typeid(Type).hash_code();
    }

protected:
    Type* d_object = nullptr;
};

/**
 * \brief Provides a basic methods and operators for shared type injection.
 */
template<typename Type>
class BindFactoryTypeSharedInjector : public BindFactoryInjector
{    
protected:
    BindFactoryTypeSharedInjector() = default;

public:
    BindFactoryTypeSharedInjector(const BindFactoryTypeSharedInjector& other) = delete;
    BindFactoryTypeSharedInjector(BindFactoryTypeSharedInjector&& other) noexcept = delete;
    BindFactoryTypeSharedInjector& operator=(const BindFactoryTypeSharedInjector& other) = delete;
    BindFactoryTypeSharedInjector& operator=(BindFactoryTypeSharedInjector&& other) noexcept = delete;
    virtual ~BindFactoryTypeSharedInjector() = default;

    /**
     * \brief Returns a shared pointer of injected instance.
     */
    const std::shared_ptr<Type>& operator->() const
    {
        return d_object;
    }

    /**
     * \brief Returns a reference of injected instance.
     */
    Type& operator*() const
    {
        return *d_object;
    }

    /**
     * \brief Checks if shared pointer of injected instance not null.
     */
    operator bool() const
    {
        return d_object != nullptr;
    }

    /**
     * \brief Checks if shared pointer of injected instance is null.
     */
    bool operator! () const
    {
        return d_object == nullptr;
    }

    /**
     * \brief Returns a pointer of injected instance from shared pointer.
     */
    Type* get() const
    {
        return d_object.get();
    }

    /**
     * \brief Returns a shared pointer of injected instance.
     */
    std::shared_ptr<Type>& get_shared()
    {
        return d_object;
    }

    /**
     * \brief Returns hash of injected.
     */
    size_t hash() override
    {
        return typeid(Type).hash_code();
    }

protected:
    std::shared_ptr<Type> d_object = nullptr;
};
