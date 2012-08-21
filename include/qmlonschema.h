#ifndef QMLON_SCHEMA_HH
#define QMLON_SCHEMA_HH

#include "qmlon.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace qmlon
{
  class Schema
  {
  public:
    static Schema& initialize(Schema& schema, qmlon::Value::Reference value);
    static Schema create(qmlon::Value::Reference value);

    Schema();
    Schema(qmlon::Value::Reference value);

    class SyntaxError : public std::runtime_error
    {
    public:
      SyntaxError(std::string const& message) : std::runtime_error(message) {}
    };

    template<typename T>
    struct Optional
    {
      Optional() : set(false), value() {}
      Optional(T const& t) : set(false), value(t) {}
      Optional<T>& operator=(T const& t) { set = true; value = t; return *this; }
      bool operator==(T const& t) { return set && value == t; }

      bool set;
      T value;
    };

    class Value
    {
    public:
      typedef std::shared_ptr<Value> Reference;
      virtual bool validate(qmlon::Value::Reference const& value) const = 0;
    };

    class Child
    {
    public:
      Child(Schema* schema) : schema(schema),  min(0), max(0), type() {}

      bool validate(qmlon::Object const* value) const;

      Optional<int> getMin() const { return min; }
      Optional<int> getMax() const { return max; }
      std::string getType() const { return type; }

      void setMin(int value) { min = value; }
      void setMax(int value) { max = value; }
      void setType(std::string value) { type = value; }

    private:
      Schema* schema;
      Optional<int> min;
      Optional<int> max;
      std::string type;
    };

    class Property {
    public:
      Property() : name(), optional(false), validTypes() {}

      bool validate(qmlon::Object const* value) const;

      std::string getName() const { return name; }
      Optional<bool> getOptional() const { return optional; }
      std::vector<Value::Reference> const& getValidTypes() const { return validTypes; }

      void setName(std::string value) { name = value; }
      void setOptional(bool value) { optional = value; }
      void addValidType(Value::Reference value) { validTypes.push_back(value); }

    private:
      std::string name;
      Optional<bool> optional;
      std::vector<Value::Reference> validTypes;
    };

    class Object
    {
    public:
      Object() : type(), isInterface(false), properties(), children() {}

      bool validate(qmlon::Object const* value) const;

      std::string getType() const { return type; }
      bool getIsInterface() const { return isInterface; }
      std::vector<Property> const& getProperties() const { return properties; }
      std::vector<Child> const& getChildren() const { return children; }

      void setType(std::string value) { type = value; }
      void setIsInterface(bool value) { isInterface = value; }
      void addProperty(Property const& value) { properties.push_back(value); }
      void addChild(Child const& value) { children.push_back(value); }

    private:
      std::string type;
      bool isInterface;
      std::vector<Property> properties;
      std::vector<Child> children;
    };

    class BooleanValue : public Value
    {
    public:
      BooleanValue() : Value() {}
      virtual bool validate(qmlon::Value::Reference const& value) const;
    };

    class IntegerValue : public Value
    {
    public:
      IntegerValue() : Value(), min(0), max(0) {}

      virtual bool validate(qmlon::Value::Reference const& value) const;

      Optional<int> getMin() const { return min; }
      Optional<int> getMax() const { return max; }

      void setMin(int value) { min = value; }
      void setMax(int value) { max = value; }

    private:
      Optional<int> min;
      Optional<int> max;
    };

    class FloatValue : public Value
    {
    public:
      FloatValue() : Value(), min(0.0f), max(0.0f) {}

      virtual bool validate(qmlon::Value::Reference const& value) const;

      Optional<float> getMin() const { return min; }
      Optional<float> getMax() const { return max; }

      void setMin(float value) { min = value; }
      void setMax(float value) { max = value; }

    private:
      Optional<float> min;
      Optional<float> max;
    };

    class StringValue : public Value
    {
    public:
      StringValue() : Value(), min(0), max(0) {}

      virtual bool validate(qmlon::Value::Reference const& value) const;

      Optional<int> getMin() const { return min; }
      Optional<int> getMax() const { return max; }

      void setMin(int value) { min = value; }
      void setMax(int value) { max = value; }

    private:
      Optional<int> min;
      Optional<int> max;
    };

    class ListValue : public Value
    {
    public:
      ListValue() : Value(), validTypes(), min(0), max(0) {}

      virtual bool validate(qmlon::Value::Reference const& value) const;

      Optional<std::vector<Value::Reference>> const& getValidTypes() const { return validTypes; }
      Optional<int> getMin() const { return min; }
      Optional<int> getMax() const { return max; }

      void addValidType(Value::Reference value) { validTypes.value.push_back(value); validTypes.set = true; }
      void setMin(int value) { min = value; }
      void setMax(int value) { max = value; }

    private:
      Optional<std::vector<Value::Reference>> validTypes;
      Optional<int> min;
      Optional<int> max;
    };

    class ObjectValue : public Value
    {
    public:
      ObjectValue(Schema* schema) : Value(), schema(schema), type() {}

      virtual bool validate(qmlon::Value::Reference const& value) const;

      Optional<std::string> getType() const { return type; }
      void setType(std::string value) { type = value; }

    private:
      Schema* schema;
      Optional<std::string> type;
    };

    void setRoot(std::string const& value) { root = value; }
    void addObject(Object const& value) { objects[value.getType()] = value; }

    std::string const& getRoot() const { return root; }
    std::map<std::string, Object> const& getObjects() const { return objects; }

    bool validate(qmlon::Value::Reference const& value) const;

  private:
    std::string root;
    std::map<std::string, Object> objects;

  };
}
#endif