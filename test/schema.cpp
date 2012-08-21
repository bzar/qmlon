#include "qmlon.h"
#include "qmlonschema.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char** argv)
{
  std::cout << "Parsing schema document schema document" << std::endl;

  std::ifstream f("schema.qmlon");
  qmlon::Value::Reference schemaDocument = qmlon::readValue(f);

  std::cout << "Creating schema document schema" << std::endl;
  qmlon::Schema schema = qmlon::Schema(schemaDocument);

  std::cout << "Validating schema document schema document" << std::endl;
  if(schema.validate(schemaDocument))
  {
    std::cout << "Schema document is valid" << std::endl;
  }
  else
  {
    std::cout << "Schema document contradicts itself!" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Parsing sprite sheet schema document" << std::endl;

  std::ifstream f2("spritesheet-schema.qmlon");
  qmlon::Value::Reference spriteSheetSchemaDocument = qmlon::readValue(f2);

  std::cout << "Validating sprite sheet schema document" << std::endl;
  if(schema.validate(spriteSheetSchemaDocument))
  {
    std::cout << "Sprite sheet schema document is valid" << std::endl;
  }
  else
  {
    std::cout << "Sprite sheet schema document is invalid!" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Creating sprite sheet schema" << std::endl;
  qmlon::Schema spriteSheetSchema = qmlon::Schema(spriteSheetSchemaDocument);

  std::cout << "Parsing sprite sheet" << std::endl;
  std::ifstream f3("spritesheet.qmlon");
  qmlon::Value::Reference spriteSheetDocument = qmlon::readValue(f3);

  std::cout << "Validating sprite sheet" << std::endl;
  if(spriteSheetSchema.validate(spriteSheetDocument))
  {
    std::cout << "Sprite sheet document is valid" << std::endl;
  }
  else
  {
    std::cout << "Sprite sheet document is invalid!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
