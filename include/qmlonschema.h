#ifndef QMLON_SCHEMA_HH
#define QMLON_SCHEMA_HH

#include <string>
#include <vector>

namespace qmlon
{
  class Schema
  {
  public:

    template<typename T>
    struct Optional
    {
      Optional() : set(false), value() {}
      Optional(T const& t) : set(false), value(t) {}
      Optional<T>& operator=(T const& t) { set = true; value = t; return *this; }

      bool set;
      T value;
    };

    class Value
    {
    };

    class Child
    {
    public:
      Child() : min(0), max(0), type() {}
      Optional<int> getMin() const { return min; }
      Optional<int> getMax() const { return max; }
      std::string getType() const { return type; }

      void setMin(int value) { min = value; }
      void setMax(int value) { max = value; }
      void setType(std::string value) { type = value; }

    private:
      Optional<int> min;
      Optional<int> max;
      std::string type;
    };

    class Property {
    public:
      Property() : name(), optional(false), validTypes() {}

      std::string getName() const { return name; }
      Optional<bool> getOptional() const { return optional; }
      std::vector<Value> getValidTypes() const { return validTypes; }

      void setName(std::string value) { name = value; }
      void setOptional(bool value) { optional = value; }
      void addValidType(Value value) { validTypes.push_back(value); }

    private:
      std::string name;
      Optional<bool> optional;
      std::vector<Value> validTypes;
    };

    class Object
    {
    public:
      Object() : isInterface(false), properties(), children() {}

      bool getIsInterface() const { return isInterface; }
      std::vector<Property> getProperties() const { return properties; }
      std::vector<Child> getChildren() const { return children; }

      void setIsInterface(bool value) { isInterface = value; }
      void addProperty(Property value) { properties.push_back(value); }
      void addChild(Child value) { children.push_back(value); }

    private:
      bool isInterface;
      std::vector<Property> properties;
      std::vector<Child> children;
    };

    class BooleanValue : public Value
    {
    public:
      BooleanValue() : Value() {}
    };

    class IntegerValue : public Value
    {
    public:
      IntegerValue() : Value(), min(0), max(0) {}

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
      StringValue() : Value(), min(0), max(0) {}
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
      Optional<std::vector<Value>> getValidTypes() const { return validTypes; }
      Optional<int> getMin() const { return min; }
      Optional<int> getMax() const { return max; }

      void addValidType(Value value) { validTypes.value.push_back(value); }
      void setMin(int value) { min = value; }
      void setMax(int value) { max = value; }

    private:
      Optional<std::vector<Value>> validTypes;
      Optional<int> min;
      Optional<int> max;
    };

    class ObjectValue : public Value
    {
    public:
      ObjectValue() : Value(), type() {}
      Optional<std::string> getType() const { return type; }
      void setType(std::string value) { type = value; }

    private:
      Optional<std::string> type;
    };
  };
}
#endif