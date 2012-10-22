QMLON - QML Object Notation parser and data structure initializer for C++11
===========================================================================

QMLON consists of three parts:
 * a parser that reads QMLON documents
 * a validation system that uses QMLON validation documents to validate other QMLON documents
 * an initializer system that creates application data structures based on parsed QMLON documents

Both are included in qmlon.h and qmlon.cpp. You can just drop these in with your other code. Note however, that you need `-std=c++0x` compiler flag (at least with GCC 4.6, `-std=c++11` for GCC 4.7 and beyond)

Reading QMLON documents is as easy as calling `qmlon::readValue` for a suitable `std::string` or `std::istream`. The function will return a qmlon::Value::Reference that represents the root object for the QMLON document. A QMLON document looks something like this:

    MyDocument {
      property1: "A string property!"
      integerProp: 123
      aFloat: 3.14
      listOfStuff: [1, 2, "foo", FunkyObject{x: 1, y: 3}, true, false]

      ChildObject {
        foo: "bar"
        objectProperty: Foo{bar: "baz"}
      }
    }

To validate the document create a QMLON validation document. A QMLON validation document is a QMLON document with a specific form. The validation document is loaded like any QMLON document and then given to `qmlon::Schema`, which can validate documents using the `qmlon::Schema::validate` method. There are two validation document examples in the `schema` directory: one to validate the sprite sheet example's QMLON document, and another to validate QMLON validation documents (including itself). To see all current features of QMLON validation documents check the latter one (no actual documentation yet). For example, the above document could be validated with the following validation document:

    Schema {
      root: "MyDocument"

      MyDocument {
        Property { name: "property1", type: String{} }
        Property { name: "integerProp", type: Integer{} }
        Property { name: "aFloat", type: Float{} }
        Property {
          name: "listOfStuff"
          type: List {
            type: [Integer{}, String{}, Boolean{}, Object{ type: "FunkyObject" }]
          }
        }

        Child { type: "ChildObject" }
      }

      ChildObject {
        Property { name: "foo", type: String{} }
        Property { name: "objectProperty", type: Object{ type: "Foo" }, optional: true }
      }

      FunkyObject {
        Property { name: "x", type: Integer{min: 1} }
        Property { name: "y", type: Integer{max: 3} }
      }

      Foo {
        Property { name: "bar", type: String{min: 3, max: 3} }
      }
    }

To use the initializer part you need to define `qmlon::Initializer` objects for each of your data structure types that represent mappings from QMLON document properties and objects to application data structures and variables. The mappings are given as "property name" -> "property initializer function object" and "child object name" -> "child initialization and insertion function object" maps. The simplest way to define these is using C++11's initializer lists for std::maps and lambda functions. For example, a part of the above document could be created into to suitable data structures using the following initializers:

    qmlon::Initializer<FooType> initFoo({
      {"bar", [](FooType& foo, qmlon::Value::Reference value) { foo.bar = value->asString(); }}
    });

    qmlon::Initializer<ChildObjectType> initChild({
      {"foo", [](ChildObjectType& child, qmlon::Value::Reference value) { child.foo = value->asString(); }}
      {"objectProperty", [](ChildObjectType& child, qmlon::Value::Reference value) { child.objectProperty = qmlon::create(value, initFoo); }}
    });

    qmlon::Initializer<MyDocumentType> initDocument({
      {"integerProp", [](MyDocumentType& doc, qmlon::Value::Reference value) { doc.setIntegerProp(value->asInteger()); }}
    }, {
      {"ChildObject", [&](MyDocumentType& doc, qmlon::Object* obj) { doc.addChild(qmlon::create(obj, initChild)); }}
    });

The `qmlon::create` creates and initializes a variable of the initializer's template type. It only works for types that are default constructible. Otherwise you'd first create the object and then use `qmlon::Initializer::init` to set the properties and children. When you've created the initializers you can just request the root type `qmlon::Initializer<MyDocumentType>` to create and initialize the data structures using `qmlon::create` or `qmlon::Initializer::init` like above.

There are also some convenience functions for common initialization operations. One such function is a generic setter generator `qmlon::set`. It takes a member or member function pointer as a parameter and returns a setter function that can be used by an initializer. For example, the above code could also be written as:

    qmlon::Initializer<FooType> initFoo({
      {"bar", qmlon::set(&FooType::bar)}
    });

    qmlon::Initializer<ChildObjectType> initChild({
      {"foo", qmlon::set(&ChildObjectType::foo)}
      {"objectProperty", [](ChildObjectType& child, qmlon::Value::Reference value) { child.objectProperty = qmlon::create(value, initFoo); }}
    });

    qmlon::Initializer<MyDocumentType> initDocument({
      {"integerProp", qmlon::set(&MyDocumentType::setIntegerProp)}
    }, {
      {"ChildObject", [&](MyDocumentType& doc, qmlon::Object* obj) { doc.addChild(qmlon::create(obj, initChild)); }}
    });

More convenience functions will be available later on.

Check the `test` directory for a full example.
