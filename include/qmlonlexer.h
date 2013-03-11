#ifndef QMLON_LEXER
#define QMLON_LEXER
#include <string>
#include <list>
#include <istream>
#include <stdexcept>

namespace qmlon
{
  enum SymbolType { 
    LINE_COMMENT, MULTILINE_COMMENT, WHITESPACE,
    OBJECT_START, OBJECT_END, LIST_START, LIST_END, 
    VALUE_SEPARATOR, KEY_VALUE_SEPARATOR, 
    INTEGER, FLOAT, BOOLEAN, IDENTIFIER, STRING,
    UNKNOWN
  };
  
  struct StreamPosition
  {
    unsigned int character;
    unsigned int line;
    unsigned int lineCharacter;    
  };
  
  struct Symbol
  {
    SymbolType type;
    std::string content;
    StreamPosition position;
  };
  
  typedef std::list<Symbol> SymbolSequence;
  
  class SyntaxError : public std::runtime_error
  {
  public:
    SyntaxError(std::string const& message, StreamPosition position) : std::runtime_error(message), position(position) {}
    StreamPosition const position;
  };
  
  SymbolSequence lex(std::istream& stream, bool includeComments = false, bool includeWhitespace = false);
}

#endif