#include "qmlon.h"
#include "qmlonschema.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

int main(int argc, char** argv)
{
  std::ifstream f("schema.qmlon");
  qmlon::Value::Reference schema = qmlon::readValue(f);

  return EXIT_SUCCESS;
}
