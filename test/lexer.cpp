#include "qmlonlexer.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

std::string const SYMBOL_NAMES[] = {
  "LINE_COMMENT", "MULTILINE_COMMENT", "WHITESPACE",
  "OBJECT_START", "OBJECT_END", "LIST_START", "LIST_END", 
  "VALUE_SEPARATOR", "KEY_VALUE_SEPARATOR", 
  "INTEGER", "FLOAT", "BOOLEAN", "IDENTIFIER", "STRING",
  "UNKNOWN"
};

int main()
{
  std::ifstream stream("schema.qmlon");
  
  try
  {
    qmlon::SymbolSequence symbols = qmlon::lex(stream);
    
    for(qmlon::Symbol const& symbol : symbols)
    {
      std::cout << symbol.position.line << ":" << symbol.position.lineCharacter << " " << SYMBOL_NAMES[symbol.type] << ": '" << symbol.content << "'" << std::endl;
    }
  }
  catch(qmlon::SyntaxError e)
  {
    std::cerr << e.what() << ", character " << e.position.character << ", line " << e.position.line << ", line character " << e.position.lineCharacter << std::endl;
  }
  
  return EXIT_SUCCESS;
}
