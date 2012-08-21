#include "qmlon.h"
#include <cctype>
#include <sstream>

namespace qmlon
{
  std::string readString(std::istream& stream);
  Value::List readList(std::istream& stream);
  Object::Reference readObject(std::string const& type, std::istream& stream);
  void printObject(Object* object, std::ostream& out = std::cout, int level = 0);
  void printValue(Value const& value, std::ostream& out = std::cout, int level = 0);
}


std::string qmlon::Value::str() const
{
  std::ostringstream ss;
  printValue(*this, ss);
  return ss.str();
}

std::string qmlon::readString(std::istream& stream)
{
  std::ostringstream buffer;
  bool escape = false;
  char c = '\0';

  while(stream.get(c) && (escape || c != '"'))
  {
    if(c == '\\' && !escape)
    {
      escape = true;
      continue;
    }

    buffer << c;
  }

  return buffer.str();
}

qmlon::Value::List qmlon::readList(std::istream& stream)
{
  Value::List list;
  char c = '\0';

  while(stream.peek() != ']')
  {
    if(isspace(stream.peek()) || stream.peek() == ',')
    {
      stream.get(c);
    }
    else
    {
      auto v = readValue(stream);
      list.push_back(v);
    }
  }

  stream.get(c);
  return list;
}

qmlon::Value::Reference qmlon::readValue(std::istream& stream)
{
  std::ostringstream buffer;
  bool canBeInteger = true;
  bool canBeFloat = false;
  char c = '\0';

  while(stream.get(c))
  {
    if(std::isblank(c))
    {
      // tab or space
      continue;
    }
    else if(std::isspace(c) || c == ',')
    {
      // line break or comma
      break;
    }
    else if(c == '"')
    {
      return Value::Reference(new StringValue(readString(stream)));
    }
    else if(c == '[')
    {
      return Value::Reference(new ListValue(readList(stream)));
    }
    else if(c == '{')
    {
      return Value::Reference(new ObjectValue(readObject(buffer.str(), stream)));
    }
    else if(c == '}')
    {
      stream.putback(c);
      break;
    }
    else
    {
      if(!std::isdigit(c))
      {
        canBeInteger = false;
        canBeFloat = (c == '.' && !canBeFloat);
      }
      buffer << c;
    }
  }

  std::string str = buffer.str();

  if(str.empty())
  {
    std::ostringstream ss;
    ss << "ERROR: Invalid value: " << buffer.str();
    throw std::runtime_error(ss.str());
  }
  else if(str == "true" || str == "false")
  {
    return Value::Reference(new BooleanValue(str == "true"));
  }
  else if(canBeInteger)
  {
    return Value::Reference(new IntegerValue(std::atoi(str.data())));
  }
  else if(canBeFloat)
  {
    return Value::Reference(new FloatValue(std::atof(str.data())));
  }
  else
  {
    std::ostringstream ss;
    ss << "ERROR: Invalid value: " << buffer.str();
    throw std::runtime_error(ss.str());
  }
}

qmlon::Value::Reference qmlon::readValue(std::string const& str)
{
  std::istringstream ss(str);
  return readValue(ss);
}

qmlon::Object::Reference qmlon::readObject(std::string const& type, std::istream& stream)
{
  Object::Reference object(new Object);
  object->type = type;
  std::ostringstream buffer;
  char c = '\0';

  while(stream.get(c) && c != '}')
  {
    if(std::isspace(c) || c == ',')
    {
      continue;
    }
    if(c == ':')
    {
      object->properties[buffer.str()] = readValue(stream);
      buffer.str("");
    }
    else if(c == '{')
    {
      object->children.push_back(readObject(buffer.str(), stream));
      buffer.str("");
    }
    else
    {
      buffer << c;
    }
  }

  return object;
}


void qmlon::printObject(Object* object, std::ostream& out, int level)
{
  out << object->type << " {" << std::endl;

  for(auto prop : object->properties)
  {
    out << std::string(level + 1, ' ');
    out << prop.first << ": ";
    printValue(*prop.second, out, level + 1);
    out << std::endl;
  }

  for(auto child : object->children)
  {
    out << std::string(level + 1, ' ');
    printObject(child.get(), out, level + 1);
  }

  out << std::string(level, ' ') << "}" << std::endl;
}

void qmlon::printValue(Value const& value, std::ostream& out, int level)
{
  if(value.isObject())
  {
    printObject(value.asObject(), out, level);
  }
  else if(value.isBoolean())
  {
    out << value.asBoolean();
  }
  else if(value.isInteger())
  {
    out << value.asInteger();
  }
  else if(value.isFloat())
  {
    out << value.asFloat();
  }
  else if(value.isString())
  {
    out << '"' << value.asString() << '"';
  }
  else if(value.isList())
  {
    out << "[ ";
    for(auto item : value.asList())
    {
      printValue(*item, out, level + 1);
      out << " ";
    }
    out << "]" << std::endl;
  }
}

