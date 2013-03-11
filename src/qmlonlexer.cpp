#include "qmlonlexer.h"
#include <sstream>

class ContextStreamWrapper
{
public:
  ContextStreamWrapper(std::istream* stream);
  explicit operator bool() const;
  ContextStreamWrapper& get(char& c);
  ContextStreamWrapper const& peek(char& c) const;
  char get();
  char peek() const;
  
  unsigned int currentPosition();
  unsigned int currentLine();
  unsigned int currentLinePosition();
  
private:
  std::istream* stream;
  unsigned int position;
  unsigned int line;
  unsigned int linePosition;
};

qmlon::SymbolSequence lex(ContextStreamWrapper& stream, bool includeComments, bool includeWhitespace);
void readString(qmlon::Symbol& symbol, ContextStreamWrapper& stream);
void readNumber(qmlon::Symbol& symbol, ContextStreamWrapper& stream);
void readComment(qmlon::Symbol& symbol, ContextStreamWrapper& stream);
void readIdentifierOrBoolean(qmlon::Symbol& symbol, ContextStreamWrapper& stream);
void readWhitespace(qmlon::Symbol& symbol, ContextStreamWrapper& stream);

qmlon::SymbolSequence qmlon::lex(std::istream& stream, bool includeComments, bool includeWhitespace)
{
  ContextStreamWrapper wrapper(&stream);
  return lex(wrapper, includeComments, includeWhitespace);
}

ContextStreamWrapper::ContextStreamWrapper(std::istream* stream) : stream(stream), position(0), line(0), linePosition(0) {}
ContextStreamWrapper::operator bool() const
{
  return *stream && std::char_traits<char>::not_eof(peek());
}
ContextStreamWrapper& ContextStreamWrapper::get(char& c)
{
  c = get();
  return *this;
}
char ContextStreamWrapper::get()
{
  char c = stream->get();
  position += 1;
  if(c == '\n')
  {
    line += 1;
    linePosition = 0;
  }
  else
  {
    linePosition += 1;
  }
  
  return c;
}
ContextStreamWrapper const& ContextStreamWrapper::peek(char& c) const
{
  c = peek();
  return *this;
}
char ContextStreamWrapper::peek() const
{
  return stream->peek();
}

unsigned int ContextStreamWrapper::currentPosition()
{
  return position;
}
unsigned int ContextStreamWrapper::currentLine()
{
  return line;
}

unsigned int ContextStreamWrapper::currentLinePosition()
{
  return linePosition;
}
  
qmlon::SymbolSequence lex(ContextStreamWrapper& stream, bool includeComments, bool includeWhitespace)
{
  qmlon::SymbolSequence symbols;
  
  char c = '\0';
  while(stream.peek(c))
  {
    qmlon::Symbol symbol{qmlon::UNKNOWN, std::string(1, c), {stream.currentPosition(), stream.currentLine(), stream.currentLinePosition()}};
    if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
    {
      readWhitespace(symbol, stream);
    }
    else if(c == '{')
    {
      stream.get();
      symbol.type = qmlon::OBJECT_START;
    }
    else if(c == '}')
    {
      stream.get();
      symbol.type = qmlon::OBJECT_END;
    }
    else if(c == '[')
    {
      stream.get();
      symbol.type = qmlon::LIST_START;
    }
    else if(c == ']')
    {
      stream.get();
      symbol.type = qmlon::LIST_END;
    }
    else if(c == ',')
    {
      stream.get();
      symbol.type = qmlon::VALUE_SEPARATOR;
    }
    else if(c == ':')
    {
      stream.get();
      symbol.type = qmlon::KEY_VALUE_SEPARATOR;
    }
    else if((c >= '0' && c <= '9') || c == '.' || c == '-')
    {
      readNumber(symbol, stream);
    }
    else if(c == '"')
    {
      readString(symbol, stream);
    }
    else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    {
      readIdentifierOrBoolean(symbol, stream);
    }
    else if(c == '/')
    {
      readComment(symbol, stream);
    }
    
    
    if(symbol.type == qmlon::UNKNOWN)
    {
      throw qmlon::SyntaxError("Invalid syntax", symbol.position);
    }
    
    if((includeComments || symbol.type != qmlon::LINE_COMMENT)
      && (includeComments || symbol.type != qmlon::MULTILINE_COMMENT)
      && (includeWhitespace || symbol.type != qmlon::WHITESPACE))
    { 
      symbols.push_back(symbol);
    }
  }
  
  return symbols;
}

void readString(qmlon::Symbol& symbol, ContextStreamWrapper& stream)
{
  symbol.type = qmlon::STRING;
  char c = stream.get();
  std::ostringstream ss;
  ss << c;
  
  while(stream.get(c) && c != '"')
  {
    ss << c;
    if(c == '\\')
    {
      stream.get();
    }
  }

  ss << c;
  symbol.content = ss.str();  
}

void readNumber(qmlon::Symbol& symbol, ContextStreamWrapper& stream)
{
  symbol.type = qmlon::INTEGER;
  char c = stream.peek();
  std::ostringstream ss;
  
  do
  {
    if(c == '.')
    {
      if(symbol.type == qmlon::FLOAT)
      {
        throw qmlon::SyntaxError("Multiple decimal points in a float", symbol.position);
      }
      
      symbol.type = qmlon::FLOAT;
    }
    ss << c;
    stream.get();
    c = stream.peek();
  } while((c >= '0' && c <= '9') || c == '.');
  
  symbol.content = ss.str();
}

void readComment(qmlon::Symbol& symbol, ContextStreamWrapper& stream)
{
  char c = stream.get();
  std::ostringstream ss;
  ss << c;
  c = stream.get();

  if(c == '/')
  {
    symbol.type = qmlon::LINE_COMMENT;
    do
    {
      ss << c;
    } while(stream.get(c) && c != '\n' && c != '\r');
  }
  else if(c == '*')
  {
    symbol.type = qmlon::MULTILINE_COMMENT;
    bool previousWasAsterisk = false;
    do
    {
      ss << c;
      previousWasAsterisk = c == '*';
    } while(stream.get(c) && !(previousWasAsterisk && c == '/'));
    ss << c;
  }

  symbol.content = ss.str();  
}

void readIdentifierOrBoolean(qmlon::Symbol& symbol, ContextStreamWrapper& stream)
{
  char c = stream.peek();
  std::ostringstream ss;
  
  do
  {
    ss << stream.get();
  } while(stream.peek(c) && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')));
  
  symbol.content = ss.str();

  if(symbol.content == "true" || symbol.content == "false")
  {
    symbol.type = qmlon::BOOLEAN;
  }
  else
  {
    symbol.type = qmlon::IDENTIFIER;
  }
}

void readWhitespace(qmlon::Symbol& symbol, ContextStreamWrapper& stream)
{
  symbol.type = qmlon::WHITESPACE;
  char c = stream.peek();
  std::ostringstream ss;
  
  do
  {
    ss << stream.get();
  } while(stream.peek(c) && (c == ' ' || c == '\t' || c == '\r' || c == '\n'));
  
  symbol.content = ss.str();
}
