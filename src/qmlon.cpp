#include "qmlon.h"
#include "qmlonlexer.h"
#include <cctype>
#include <sstream>
#include <fstream>
#include <iostream>
namespace qmlon
{
  Value::Reference readValue(SymbolSequence& symbols);
  Value::List readList(SymbolSequence& symbols);
  Object::Reference readObject(SymbolSequence& symbols);
  Object::Reference readObject(SymbolSequence& symbols, std::string const& type);
  void printObject(Object& object, std::ostream& out = std::cout, int level = 0);
  void printValue(Value const& value, std::ostream& out = std::cout, int level = 0);
}


std::string qmlon::Value::str() const
{
  std::ostringstream ss;
  printValue(*this, ss);
  return ss.str();
}

qmlon::Value::List qmlon::readList(SymbolSequence& symbols)
{
  Symbol symbol = symbols.front();
  if(symbol.type != LIST_START)
  {
    std::ostringstream ss;
    ss << "ERROR: Expected [ at line " << symbol.position.line + 1 << " character " << symbol.position.lineCharacter + 1;
    throw std::runtime_error(ss.str());
  }

  symbols.pop_front();
  Value::List list;

  while(symbols.front().type != LIST_END)
  {
    if(symbols.front().type == VALUE_SEPARATOR)
    {
      symbols.pop_front();
    }
    
    auto v = readValue(symbols);
    list.push_back(v);
  }

  symbols.pop_front();
  return list;
}

qmlon::Value::Reference qmlon::readValue(std::istream& stream)
{
  SymbolSequence symbols = lex(stream);
  return readValue(symbols);
}

qmlon::Value::Reference qmlon::readValue(SymbolSequence& symbols) 
{
  Symbol symbol = symbols.front();
  
  if(symbol.type == IDENTIFIER || symbol.type == OBJECT_START)
  {
    return Value::Reference(new ObjectValue(readObject(symbols)));
  }
  else if(symbol.type == LIST_START)
  {
    return Value::Reference(new ListValue(readList(symbols)));
  }
  else if(symbol.type == INTEGER)
  {
    symbols.pop_front();
    return Value::Reference(new IntegerValue(std::atoi(symbol.content.data())));
  }
  else if(symbol.type == FLOAT)
  {
    symbols.pop_front();
    return Value::Reference(new FloatValue(std::atof(symbol.content.data())));
  }
  else if(symbol.type == BOOLEAN)
  {
    symbols.pop_front();
    return Value::Reference(new BooleanValue(symbol.content == "true"));
  }
  else if(symbol.type == STRING)
  {
    symbols.pop_front();
    // Remove quotes from string value
    std::string stringValue = symbol.content.substr(1, symbol.content.length() - 2);
    return Value::Reference(new StringValue(stringValue));
  }
  else
  {
    std::ostringstream ss;
    ss << "ERROR: Invalid value at line " << symbol.position.line + 1 << " character " << symbol.position.lineCharacter + 1;
    throw std::runtime_error(ss.str());
  }
}

qmlon::Value::Reference qmlon::readValue(std::string const& str)
{
  std::istringstream ss(str);
  return readValue(ss);
}

qmlon::Value::Reference qmlon::readFile(std::string const& filename)
{
  std::ifstream ss(filename);
  return readValue(ss);
}

qmlon::Object::Reference qmlon::readObject(SymbolSequence& symbols)
{
  std::string type;
  Symbol symbol = symbols.front();
  if(symbol.type == IDENTIFIER)
  {
    type = symbol.content;
    symbols.pop_front();
  }

  return readObject(symbols, type);
}

qmlon::Object::Reference qmlon::readObject(SymbolSequence& symbols, std::string const& type)
{
  Symbol symbol = symbols.front();
  
  if(symbol.type != OBJECT_START)
  {
    std::ostringstream ss;
    ss << "ERROR: Expected { at line " << symbol.position.line + 1 << " character " << symbol.position.lineCharacter + 1;
    throw std::runtime_error(ss.str());
  }
  
  // pop OBJECT_START
  symbols.pop_front();
  
  Object::Reference object(new Object);
  object->type = type;

  while(symbols.front().type != OBJECT_END)
  {
    symbol = symbols.front();
    
    if(symbol.type == VALUE_SEPARATOR)
    {
      symbols.pop_front();
      symbol = symbols.front();
    }
    
    if(symbol.type == OBJECT_START)
    {
      object->children.push_back(readObject(symbols, ""));
    }
    else if(symbol.type == IDENTIFIER)
    {
      Symbol identifierSymbol = symbols.front();
      
      // pop IDENTIFIER
      symbols.pop_front();
      symbol = symbols.front();
      
      if(symbol.type == KEY_VALUE_SEPARATOR)
      {
        // pop KEY_VALUE_SEPARATOR
        symbols.pop_front();
        object->properties[identifierSymbol.content] = readValue(symbols);
      }
      else if(symbol.type == OBJECT_START)
      {
        object->children.push_back(readObject(symbols, identifierSymbol.content));
      }
      else
      {
        std::ostringstream ss;
        ss << "ERROR: Expected property or child object at line " << symbol.position.line + 1 << " character " << symbol.position.lineCharacter + 1;
        throw std::runtime_error(ss.str());
      }
    }
  }
  
  // pop OBJECT_END
  symbols.pop_front();

  return object;
}


void qmlon::printObject(Object& object, std::ostream& out, int level)
{
  out << object.type << " {" << std::endl;

  for(auto prop : object.properties)
  {
    out << std::string(level + 1, ' ');
    out << prop.first << ": ";
    printValue(*prop.second, out, level + 1);
    out << std::endl;
  }

  for(auto child : object.children)
  {
    out << std::string(level + 1, ' ');
    printObject(*child, out, level + 1);
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
