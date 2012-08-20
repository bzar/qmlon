#include "qmlon.h"
#include "qmlonschema.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char** argv)
{
  std::ifstream f("schema.qmlon");
  qmlon::Value::Reference schemaDocument = qmlon::readValue(f);
  qmlon::Schema schema = qmlon::Schema(schemaDocument);

  std::cout << "Schema" << std::endl
            << "  root: " << schema.getRoot() << std::endl
            << "  object types (" << schema.getObjects().size() << ")" << std::endl;

  for(auto ot : schema.getObjects())
  {
    std::string objectType = ot.first;
    qmlon::Schema::Object object = ot.second;
    std::cout << "    " << objectType << std::endl
              << "      interface: " << object.getIsInterface() << std::endl
              << "      properties (" << object.getProperties().size() << ")" << std::endl;

    for(auto p : object.getProperties())
    {
      std::cout << "        " << p.getName() << std::endl;
      std::cout << "          optional: "
                << (p.getOptional().set && p.getOptional().value ? "yes" : "no")
                << std::endl;
    }

    std::cout << "      child types (" << object.getChildren().size() << ")" << std::endl;
    for(auto c : object.getChildren())
    {
      std::cout << "        " << c.getType() << std::endl;
    }
  }
  return EXIT_SUCCESS;
}
