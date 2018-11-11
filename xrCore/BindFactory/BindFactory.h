#pragma once
#include <map>
#include <typeinfo>
#include <vector>
#include "BindFactoryRegistrator.h"
#include "BindFactoryInjector.h"

/**
 * \brief Provides interface for dependency injection.
 */
class BindFactory
{
public:
    using Injector = BindFactoryInjector;
    using InjectorsVector = std::vector<Injector*>;

    BindFactory(const BindFactory& other) = delete;
    BindFactory(BindFactory&& other) = delete;
    BindFactory& operator=(const BindFactory& other) = delete;
    BindFactory& operator=(BindFactory&& other) = delete;
    BindFactory() = default;

    ~BindFactory()
    {
        auto it = d_registrators.begin();
        auto ite = d_registrators.end();
        while (it != ite)
            delete it++->second;
    }

    /**
     * \brief Registers type in the factory.
     * \tparam T Type to register in factory
     * \return Registrator for current type
     */
    template<typename T>
    BindFactoryTypeRegistrator<T>* bind()
    {
        size_t hash = typeid(T).hash_code();

        if (d_registrators[hash] != nullptr)
            throw std::exception("Attempt to bind already binded object");

        auto registrator = new BindFactoryTypeRegistrator<T>(*this);
        d_registrators[hash] = registrator;
        return registrator;
    }

    /**
     * \brief Retrieves instance of current type from factory.
     * \tparam T Type of retrieving instance.
     * \tparam Args Types of arguments that passed to provider / constructor of class.
     * \param args Arguments that passed to provider / constructor of class.
     * \return Instance of type T.
     */
    template<typename T, typename ... Args>
    T* get(Args ... args)
    {
        size_t hash = typeid(T).hash_code();
        auto entry = d_registrators[hash];

        if (entry != nullptr)
            return entry->implementation<T>()->get(std::forward<Args>(args)...);

        return nullptr;
    }

    /**
     * \brief Retrieves instance of type T from factory and cast it to type V via static_cast.
     * \tparam T Type of retrieving instance.
     * \tparam V Type of returning value.
     * \tparam Args Types of arguments that passed to provider / constructor of class.
     * \param args Arguments that passed to provider / constructor of class.
     * \return Instance of type T.
     */
    template<typename T, typename V, typename ... Args>
    V* get(Args ... args)
    {
        return static_cast<V*>(get<T>(std::forward<Args>(args)...));
    }

    /**
     * \brief Retrieves a shared instance of current type from factory.
     * \tparam T Type of retrieving instance.
     * \tparam Args Types of arguments that passed to provider / constructor of class.
     * \param args Arguments that passed to provider / constructor of class.
     * \return Shared instance of type T.
     */
    template<typename T, typename ... Args>
    std::shared_ptr<T> getShared(Args ... args)
    {
        size_t hash = typeid(T).hash_code();
        auto entry = d_registrators[hash];

        if (entry != nullptr)
            return entry->implementation<T>()->getShared(std::forward<Args>(args)...);

        return nullptr;
    }

    /**
     * \brief Retrieves a shared instance of type T from factory and cast it to type V via static_cast.
     * \tparam T Type of retrieving instance.
     * \tparam V Type of returning value.
     * \tparam Args Types of arguments that passed to provider / constructor of class.
     * \param args Arguments that passed to provider / constructor of class.
     * \return Shared instance of type T.
     */
    template<typename T, typename V, typename ... Args>
    std::shared_ptr<V> getShared(Args ... args)
    {
        size_t hash = typeid(T).hash_code();
        auto entry = d_registrators[hash];

        if (entry != nullptr)
            return entry->implementation<T>()->getShared(std::forward<Args>(args)...);

        return nullptr;
    }

    /**
     * \brief Removes registration of type T from factory.     
     */
    template<typename T>
    void unbind()
    {
        size_t hash = typeid(T).hash_code();

        if (d_registrators[hash] != nullptr)
        {
            auto it = d_injectors[hash].begin();
            auto end = d_injectors[hash].end();
            while (it != end)
                (*it++)->uninject();

            delete d_registrators[hash];
            d_registrators.erase(hash);
        }
    }

    /**
     * \brief Internal method. Registers injector for late injection or injects immediately 
     * if injection type already registered in factory.     
     */
    void bind(Injector* injector)
    {
        size_t hash = injector->hash();
        d_injectors[hash].push_back(injector);
        if (d_registrators[hash] != nullptr)
            injector->inject();
    }

    /**
     * \brief Internal method. Unregisters injector from factory.     
     */
    void unbind(Injector* injector)
    {
        size_t hash = injector->hash();

        auto& vector = d_injectors[hash];
        auto result = std::find(vector.begin(), vector.end(), injector);

        if (result != vector.end())
            vector.erase(result);
    }

    /**
     * \brief Internal method. Returns registered injectors for specific type.
     */
    template<typename Type>
    InjectorsVector& getInjectorsForType()
    {
        size_t hash = typeid(Type).hash_code();
        return d_injectors[hash];
    }

private:
    std::map<size_t, BindFactoryRegistrator*> d_registrators;
    std::map<size_t, InjectorsVector> d_injectors;
};

/**
 * \brief Internal method. Injecting all injectors of specific type.
 */
template <typename T>
void BindFactoryTypeRegistrator<T>::injectInjectors() const
{
    auto& injectors = d_factory.getInjectorsForType<T>();
    auto it = injectors.begin();
    auto end = injectors.end();
    while (it != end)
        (*it++)->inject();
}
