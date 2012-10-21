#ifndef QMLON_INITIALIZER_HH
#define QMLON_INITIALIZER_HH

#include "qmlon.h"
#include <type_traits>

namespace qmlon
{
template<class T>
  class Initializer
  {
  public:
    Initializer(std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters = std::map<std::string, std::function<void(T&, Value::Reference)>>(),
                std::map<std::string, std::function<void(T&, Object*)>> childSetters = std::map<std::string, std::function<void(T&, Object*)>>());

    void addPropertySetter(std::string const& name, std::function<void(T&, Value::Reference)> setter);
    void addChildSetter(std::string const& name, std::function<void(T&, Object*)> setter);

    T& init(T& t, Object* obj);
    T& init(T& t, Value::Reference value);
    
    T create(Object* obj, typename std::enable_if<std::is_default_constructible<T>::value>::type* = 0);
    T create(Value::Reference value, typename std::enable_if<std::is_default_constructible<T>::value>::type* = 0);
    
    static T& initialize(T& t, Object* obj,
                          std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters,
                          std::map<std::string, std::function<void(T&, Object*)>> childSetters);
  private:
    std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters;
    std::map<std::string, std::function<void(T&, Object*)>> childSetters;
  };

  template<class T>
  Initializer<T>::Initializer(std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters,
                              std::map<std::string, std::function<void(T&, Object*)>> childSetters) :
    propertySetters(propertySetters), childSetters(childSetters)
  {}

  template<class T>
  void Initializer<T>::addPropertySetter(std::string const& name, std::function<void(T&, Value::Reference)> setter)
  {
    propertySetters[name] = setter;
  }

  template<class T>
  void Initializer<T>::addChildSetter(std::string const& name, std::function<void(T&, Object*)> setter)
  {
    childSetters[name] = setter;
  }

  template<class T>
  T& Initializer<T>::init(T& t, Object* obj)
  {
    return initialize(t, obj, propertySetters, childSetters);
  }

  template<class T>
  T& Initializer<T>::init(T& t, Value::Reference value)
  {
    return init(t, value->asObject());
  }

  template<class T>
  T Initializer<T>::create(Object* obj, typename std::enable_if<std::is_default_constructible<T>::value>::type*)
  {
    T t;
    return init(t, obj);
  }
  
  template<class T>
  T Initializer<T>::create(Value::Reference value, typename std::enable_if<std::is_default_constructible<T>::value>::type*)
  {
    T t;
    return init(t, value);
  }

  template<class T>
  T& Initializer<T>::initialize(T& t, Object* obj,
                std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters,
                std::map<std::string, std::function<void(T&, Object*)>> childSetters)
  {
    for(auto keyValuePair : obj->properties)
    {
      auto setter = propertySetters.find(keyValuePair.first);
      if(setter != propertySetters.end())
      {
        (setter->second )(t, keyValuePair.second);
      }
    }

    for(auto child : obj->children)
    {
      auto setter = childSetters.find(child->type);
      if(setter != childSetters.end())
      {
        (setter->second)(t, child.get());
      }
      else
      {
        auto defaultSetter = childSetters.find("");
        if(defaultSetter != childSetters.end())
        {
          (defaultSetter->second)(t, child.get());
        }
      }
    }
    return t;
  }
}

#endif