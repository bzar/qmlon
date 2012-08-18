#ifndef QMLON_HH
#define QMLON_HH

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <vector>
#include <stdexcept>
#include <memory>

namespace qmlon
{
  class Object;

  class Value
  {
  public:
    typedef std::shared_ptr<Value> Reference;
    typedef std::vector<Reference> List;

    virtual bool isBoolean() const { return false; }
    virtual bool isInteger() const { return false; }
    virtual bool isFloat() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isObject() const { return false; }
    virtual bool isList() const { return false; }

    virtual bool asBoolean() const { throw std::runtime_error("Invalid use of QMLON value. Value type is not boolean!"); }
    virtual int asInteger() const { throw std::runtime_error("Invalid use of QMLON value. Value type is not integer!"); }
    virtual float asFloat() const { throw std::runtime_error("Invalid use of QMLON value. Value type is not float!"); }
    virtual std::string const& asString() const { throw std::runtime_error("Invalid use of QMLON value. Value type is not string!"); }
    virtual Object* asObject() const { throw std::runtime_error("Invalid use of QMLON value. Value type is not object!"); }
    virtual List const& asList() const { throw std::runtime_error("Invalid use of QMLON value. Value type is not list!"); }

    std::string str() const;

  private:
    static std::string const NULL_STRING;
    static List const NULL_LIST;
  };

  class Object
  {
  public:
    typedef std::shared_ptr<Object> Reference;
    typedef std::map<std::string, Value::Reference> Properties;
    typedef std::vector<Object::Reference> Children;

    Object() : type(), properties(), children() {}

    bool hasProperty(std::string const& name) const { properties.find(name) != properties.end(); }
    Value::Reference getProperty(std::string const& name){ return properties.find(name)->second; }

    std::string type;
    Properties properties;
    Children children;
  };

  class BooleanValue : public Value
  {
  public:
    BooleanValue(bool value) : value(value) {}
    bool isBoolean() const { return true; }
    bool asBoolean() const { return value; }
  private:
    bool value;
  };

  class IntegerValue : public Value
  {
  public:
    IntegerValue(int value) : value(value) {}
    bool isInteger() const { return true; }
    int asInteger() const { return value; }
  private:
    int value;
  };

  class FloatValue : public Value
  {
  public:
    FloatValue(float value) : value(value) {}
    bool isFloat() const { return true; }
    float asFloat() const { return value; }
  private:
    float value;
  };

  class StringValue : public Value
  {
  public:
    StringValue(std::string const& value) : value(value) {}
    bool isString() const { return true; }
    std::string const& asString() const { return value; }
  private:
    std::string value;
  };

  class ObjectValue : public Value
  {
  public:
    ObjectValue(Object::Reference value) : value(value) {}
    bool isObject() const { return true; }
    Object* asObject() const { return value.get(); }
  private:
    Object::Reference value;
  };

  class ListValue : public Value
  {
  public:
    ListValue(List value) : value(value) {}
    bool isList() const { return true; }
    List const& asList() const { return value; }
  private:
    List value;
  };

  template<class T>
  class Initializer
  {
  public:
    Initializer(std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters = std::map<std::string, std::function<void(T&, Value::Reference)>>(),
                std::map<std::string, std::function<void(T&, Object*)>> childSetters = std::map<std::string, std::function<void(T&, Object*)>>());

    void init(T& t, Object* obj);
    void init(T& t, Value::Reference value);

    static void initialize(T& t, Object* obj,
                          std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters,
                          std::map<std::string, std::function<void(T&, Object*)>> childSetters);
  private:
    std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters;
    std::map<std::string, std::function<void(T&, Object*)>> childSetters;
  };

  Value::Reference readValue(std::istream& stream);
  Value::Reference readValue(std::string const& str);

  template<class T>
  Initializer<T>::Initializer(std::map<std::string, std::function<void(T&, Value::Reference)>> propertySetters,
                              std::map<std::string, std::function<void(T&, Object*)>> childSetters) :
    propertySetters(propertySetters), childSetters(childSetters)
  {}

  template<class T>
  void Initializer<T>::init(T& t, Object* obj)
  {
    initialize(t, obj, propertySetters, childSetters);
  }

  template<class T>
  void Initializer<T>::init(T& t, Value::Reference value)
  {
    initialize(t, value->asObject(), propertySetters, childSetters);
  }

  template<class T>
  void Initializer<T>::initialize(T& t, Object* obj,
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
    }
  }

}

#endif