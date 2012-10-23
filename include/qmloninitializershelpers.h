#ifndef QMLON_INITIALIZER_HELPERS
#define QMLON_INITIALIZER_HELPERS

#include "qmloninitializer.h"

namespace qmlon
{
  template<class T> T create(Object* obj, Initializer<T>& initializer);
  template<class T> T create(Value::Reference value, Initializer<T>& initializer);
  
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(bool));
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(int));
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(float));
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(std::string));
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(bool const&));
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(int const&));
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(float const&));
  template<class T, typename R> std::function<void(T&, qmlon::Value::Reference)> set(R (T::*setter)(std::string const&));
  
  template<class T> std::function<void(T&, qmlon::Value::Reference)> set(bool T::*value);
  template<class T> std::function<void(T&, qmlon::Value::Reference)> set(int T::*value);
  template<class T> std::function<void(T&, qmlon::Value::Reference)> set(float T::*value);
  template<class T> std::function<void(T&, qmlon::Value::Reference)> set(std::string T::*value);

  template<class T, class U> std::function<void(T&, qmlon::Value::Reference)> createSet(Initializer<U>& initializer, U T::*value);
  template<class T, class U, typename R> std::function<void(T&, qmlon::Value::Reference)> createSet(Initializer<U>& initializer, R (T::*setter)(U));
  template<class T, class U, typename R> std::function<void(T&, qmlon::Value::Reference)> createSet(Initializer<U>& initializer, R (T::*setter)(U const&));
  template<class T, class U> std::function<void(T&, qmlon::Value::Reference)> createSet(std::initializer_list<Initializer<U>> initializers, U T::*value);
  template<class T, class U, typename R> std::function<void(T&, qmlon::Value::Reference)> createSet(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U));
  template<class T, class U, typename R> std::function<void(T&, qmlon::Value::Reference)> createSet(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U const&));
  
  template<class T, class U, typename R> std::function<void(T&, qmlon::Object*)> createAdd(Initializer<U>& initializer, R (T::*setter)(U));
  template<class T, class U, typename R> std::function<void(T&, qmlon::Object*)> createAdd(Initializer<U>& initializer, R (T::*setter)(U const&));
  template<class T, class U, typename R> std::function<void(T&, qmlon::Object*)> createAdd(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U));
  template<class T, class U, typename R> std::function<void(T&, qmlon::Object*)> createAdd(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U const&));

  ///////////////////////////////////////////////////////////////////
  
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

  template<class T, class U>
  std::function<void(T&, qmlon::Value::Reference)> createSet(Initializer<U>& initializer, U T::*value)
  {
    return [&initializer, value](T& t, qmlon::Value::Reference v) { 
      U u;
      initializer.init(u, v);
      (t.*value) = u;
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Value::Reference)> createSet(Initializer<U>& initializer, R (T::*setter)(U))
  {
    return [&initializer, setter](T& t, qmlon::Value::Reference v) { 
      U u;
      initializer.init(u, v);
      (t.*setter)(u);
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Value::Reference)> createSet(Initializer<U>& initializer, R (T::*setter)(U const&))
  {
    return [&initializer, setter](T& t, qmlon::Value::Reference v) { 
      U u;
      initializer.init(u, v);
      (t.*setter)(u);
    };
  }

  template<class T, class U>
  std::function<void(T&, qmlon::Value::Reference)> createSet(std::initializer_list<Initializer<U>> initializers, U T::*value)
  {
    return [&initializers, value](T& t, qmlon::Value::Reference v) { 
      U u;
      for(auto initializer : initializers) {
	initializer.init(u, v);
      }
      (t.*value) = u;
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Value::Reference)> createSet(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U))
  {
    return [&initializers, setter](T& t, qmlon::Value::Reference v) { 
      U u;
      for(auto initializer : initializers) {
	initializer.init(u, v);
      }
      (t.*setter)(u);
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Value::Reference)> createSet(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U const&))
  {
    return [&initializers, setter](T& t, qmlon::Value::Reference v) { 
      U u;
      for(auto initializer : initializers) {
	initializer.init(u, v);
      }
      (t.*setter)(u);
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Object*)> createAdd(Initializer<U>& initializer, R (T::*setter)(U))
  {
    return [&initializer, setter](T& t, qmlon::Object* obj) { 
      U u;
      initializer.init(u, obj);
      (t.*setter)(u);
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Object*)> createAdd(Initializer<U>& initializer, R (T::*setter)(U const&))
  {
    return [&initializer, setter](T& t, qmlon::Object* obj) { 
      U u;
      initializer.init(u, obj);
      (t.*setter)(u);
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Object*)> createAdd(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U))
  {
    return [&initializers, setter](T& t, qmlon::Object* obj) { 
      U u;
      for(auto initializer : initializers) {
	initializer.init(u, obj);
      }
      (t.*setter)(u);
    };
  }

  template<class T, class U, typename R>
  std::function<void(T&, qmlon::Object*)> createAdd(std::initializer_list<Initializer<U>> initializers, R (T::*setter)(U const&))
  {
    return [&initializers, setter](T& t, qmlon::Object* obj) { 
      U u;
      for(auto initializer : initializers) {
	initializer.init(u, obj);
      }
      (t.*setter)(u);
    };
  }
}

#endif