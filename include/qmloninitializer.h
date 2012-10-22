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
  
  template<class T>
  T create(Object* obj, Initializer<T>& initializer)
  {
    T t;
    return initializer.init(t, obj);
  }
  
  template<class T>
  T create(Value::Reference value, Initializer<T>& initializer)
  {
    T t;
    return initializer.init(t, value);
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(bool))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asBoolean()); };
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(int))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asInteger()); };
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(float))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asFloat()); };
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(std::string))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asString()); };
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(bool const&))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asBoolean()); };
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(int const&))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asInteger()); };
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(float const&))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asFloat()); };
  }

  template<class T, typename R>
  std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(std::string const&))
  {
    return [setter](T& t, qmlon::Value::Reference v) { (t.*setter)(v->asString()); };
  }

  template<class T>
  std::function<void(T&, qmlon::Value::Reference)> set(bool T::*value)
  {
    return [value](T& t, qmlon::Value::Reference v) { (t.*value) = v->asBoolean(); };
  }

  template<class T>
  std::function<void(T&, qmlon::Value::Reference)> set(int T::*value)
  {
    return [value](T& t, qmlon::Value::Reference v) { (t.*value) = v->asInteger(); };
  }

  template<class T>
  std::function<void(T&, qmlon::Value::Reference)> set(float T::*value)
  {
    return [value](T& t, qmlon::Value::Reference v) { (t.*value) = v->asFloat(); };
  }

  template<class T>
  std::function<void(T&, qmlon::Value::Reference)> set(std::string T::*value)
  {
    return [value](T& t, qmlon::Value::Reference v) { (t.*value) = v->asString(); };
  }


}

#endif
