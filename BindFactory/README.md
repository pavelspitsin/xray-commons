## Bind Factory
Bind factory is a simple dependency injection provider for c++ application. It allows you to connect different modules of your application just using interfaces.

##### Example usage of bind factory:
```cpp
// Factory instance
BindFactory g_factory;

// Declare example class interface
class IWeapon
{
public:
    virtual ~IWeapon() = default;
    virtual void kill() = 0;
};

// Declare example class interface implementation
class Rifle : public IWeapon
{
public:
    void kill() override
    {
        std::cout << "Shot's fired!" << std::endl;
    }
};

int main()
{
    // Bind interface to implementation
    g_factory.bind<IWeapon>()->asSingleton<Rifle>();

    // Retrieve interface implementation instance
    IWeapon* weapon = g_factory.get<IWeapon>();
    
    // Kill someone!
    weapon->kill();
}
```

## Injectors
Bind factory provides a base wrapper classes that automaticly updates instance from factory for binding type. For each instance of bind factory you need to create your own implementations of these injectors. 

##### Example injector class:
```cpp
// Factory instance
BindFactory g_factory;

template<typename T>
class ExampleInjector : public BindFactoryTypeInjector<T>
{
    ExampleInjector()
    {
        // register our injector in our factory
        g_factory.bind(this);
    }

    ~ExampleInjector()
    {
        // unregister our injector from our factory
        g_factory.unbind(this);
    }

    // when implementation of type T is binded in our factory this method will be called.
    void inject() override
    {
        // retrieve instance from our factory
        d_object = g_factory.get<T>();
    }

    // when implementation of type T is unbinded from our factory this method will be called.
    void uninject() override
    {
        d_object = nullptr;
    }
};
```

##### Example of injector usage:
```cpp
class Warrior
{
public:
    void fight()
    {
        m_weapon->kill();
    }

private:
    ExampleInjector<IWeapon> m_weapon;
}
```

## Providers
Bind factory has a different provides for a binding types:

| Type | Usage | Description | Destruction | Custom arguments |
| - | - | - | - | - |
| Singleton | `asSingleton<Rifle>()` | Only one instance of the type will be created, and the same instance will be returned for each request. | Only when unbind() will be called | Arguments will be passed once when constructor of class  will be called. |
| Instance | `asSingleton(new Rifle())` | Instance that passed to argument will be returned for each request. | Never. User is responsible of destruction of instance. | No |
| Thread | `asThreadSingleton<Rifle>()` | Only one instance of the type will be created per thread. | Destructor will be called when thread is destroyed. | Arguments will be passed once per thread when constructor of class will be called. |
| Transient | `asTransient<Rifle>()` | New instance of the type will be created per request. | Pointer destruction is user controller. Use getShared() for automatic instance desctructions. | Arguments will be passed each time when constructor of class will be called. |
| Custom | `asProvider()` | User manage creation of instances for requests. |User manage destruction of instance. | Custom arguments completely user controlled. |

### Custom providers
You can define your own instance provider that implements BindFactoryProvider class.

##### Example of custom provider:
``` cpp
class ExampleCustomRifleProvider : public BindFactoryProvider<IWeapon>
{
public:
    IWeapon* get(Args&& ... args) override
    {
        return new Rifle();
    }
    
    std::shared_ptr<IWeapon> get_shared(Args&& ... args) override
    {
        return std::make_shared<Rifle>();
    }
}
```

### Custom arguments
While retrieving a instance from factory you can pass arguments. Depending on provider it can be passed to class constructor or used for different purposes in custom providers. Custom argument types must be passed during a type binding. Be careful with this technique, because argument types not controlled not all and can lead to errors.

##### Example of retrieving of instance from factory with custom arguments:
```cpp
class Rifle : public IWeapon
{
public:
    Rifle(unsigned int ammoCount) : m_ammoCount(ammoCount) {}
    
    void kill() override
    {
        std::cout << m_ammoCount << " shot's fired!" << std::endl;
    }
    
private:
    unsigned int m_ammoCount;
};

int main()
{
    // Bind interface to implementation with one custom argument
    g_factory.bind<IWeapon>()->asSingleton<Rifle, unsigned int>();

    // Retrieve interface implementation instance with custom arguments
    IWeapon* weapon = g_factory.get<IWeapon>(15);
    
    // Kill someone!
    weapon->kill(); // Will print "15 shot's fired!"
}
```