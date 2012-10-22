#include "qmlonschema.h"
#include "qmloninitializer.h"

qmlon::Schema& qmlon::Schema::initialize(Schema& schema, qmlon::Value::Reference value)
{
  qmlon::Initializer<BooleanValue> bvi;

  qmlon::Initializer<IntegerValue> ivi({
    {"min", qmlon::set(&IntegerValue::setMin)},
    {"max", qmlon::set(&IntegerValue::setMax)}
  });

  qmlon::Initializer<FloatValue> fvi({
    {"min", qmlon::set(&FloatValue::setMin)},
    {"max", qmlon::set(&FloatValue::setMax)}
  });

  qmlon::Initializer<StringValue> svi({
    {"min", qmlon::set(&StringValue::setMin)},
    {"max", qmlon::set(&StringValue::setMax)}
  });

  qmlon::Initializer<ObjectValue> ovi({
    {"type", qmlon::set(&ObjectValue::setType)}
  });

  qmlon::Initializer<ListValue> lvi({
    {"min", qmlon::set(&ListValue::setMin)},
    {"max", qmlon::set(&ListValue::setMax)}
  });

  std::function<qmlon::Schema::Value::Reference(qmlon::Value::Reference)> createValue([&](qmlon::Value::Reference value) {
    qmlon::Object* o = value->asObject();
    qmlon::Schema::Value::Reference result;

    if(o->type == "Boolean")
    {
      BooleanValue* bv = new BooleanValue;
      bvi.init(*bv, o);
      result.reset(bv);
    }
    else if(o->type == "Integer")
    {
      IntegerValue* iv = new IntegerValue;
      ivi.init(*iv, o);
      result.reset(iv);
    }
    else if(o->type == "Float")
    {
      FloatValue* fv = new FloatValue;
      fvi.init(*fv, o);
      result.reset(fv);

    }
    else if(o->type == "String")
    {
      StringValue* sv = new StringValue;
      svi.init(*sv, o);
      result.reset(sv);
    }
    else if(o->type == "Object")
    {
      ObjectValue* ov = new ObjectValue(&schema);
      ovi.init(*ov, o);
      result.reset(ov);
    }
    else if(o->type == "List")
    {
      ListValue* lv = new ListValue;
      lvi.init(*lv, o);
      result.reset(lv);
    }

    if(!result)
      throw SyntaxError("ERROR: Not a Schema type");

    return result;
  });

  lvi.addPropertySetter("type", [&](ListValue& x, qmlon::Value::Reference v) {
    if(v->isList())
    {
      qmlon::Value::List l = v->asList();
      for(qmlon::Value::Reference vr : l)
      {
        x.addValidType(createValue(vr));
      }
    }
    else if(v->isObject())
    {
      x.addValidType(createValue(v));
    }
  });

  qmlon::Initializer<Property> pi({
    {"name", [](Property& x, qmlon::Value::Reference v) { x.setName(v->asString()); }},
    {"optional", [](Property& x, qmlon::Value::Reference v) { x.setOptional(v->asBoolean()); }},
    {"type", [&](Property& x, qmlon::Value::Reference v) {

      if(v->isList())
      {
        qmlon::Value::List l = v->asList();
        for(qmlon::Value::Reference vr : l)
        {
          x.addValidType(createValue(vr));
        }
      }
      else if(v->isObject())
      {
        x.addValidType(createValue(v));
      }
      else
      {
        throw SyntaxError("ERROR: Property type is invalid!");
      }
    }},
  });

  qmlon::Initializer<Child> ci({
    {"min", qmlon::set(&Child::setMin)},
    {"max", qmlon::set(&Child::setMax)},
    {"type", qmlon::set(&Child::setType)}
  });

  qmlon::Initializer<Object> oi({
    {"interface", qmlon::set(&Object::setIsInterface)}
  }, {
    {"Property", [&](Object& x, qmlon::Object* obj) { x.addProperty(qmlon::create(obj, pi)); }},
    {"Child", [&](Object& x, qmlon::Object* obj) {
        Child child(&schema);
        ci.init(child, obj);
        x.addChild(child);
    }},
  });

  qmlon::Initializer<Schema> si({
    {"root", qmlon::set(&Schema::setRoot)}
  }, {
    {"", [&](Schema& x, qmlon::Object* obj) {
      Object o;
      oi.init(o, obj);
      o.setType(obj->type);
      x.addObject(o);
    }}
  });

  si.init(schema, value);
  return schema;
}

qmlon::Schema qmlon::Schema::create(qmlon::Value::Reference value)
{
  Schema schema;
  return initialize(schema, value);
}

qmlon::Schema::Schema() :
  root(), objects()
{
}

qmlon::Schema::Schema(qmlon::Value::Reference value) :
  root(), objects()
{
  initialize(*this, value);
}

bool qmlon::Schema::Child::validate(qmlon::Object::Reference object) const
{
  auto objectType = schema->getObjects().find(type);
  if(objectType == schema->getObjects().end())
    return false;

  if(!objectType->second.getIsInterface() && object->type != type)
  {
    return false;
  }

  if(!objectType->second.validate(object.get()))
  {
    return false;
  }

  return true;
}

bool qmlon::Schema::Property::validate(qmlon::Object const* value) const
{
  auto p = value->properties.find(name);
  if(p == value->properties.end())
  {
    if(optional.set && optional.value)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  for(qmlon::Schema::Value::Reference validType : validTypes)
  {
    if(validType->validate(p->second))
      return true;
  }

  return false;
}

bool qmlon::Schema::Object::validate(qmlon::Object const* value) const
{
  if(!isInterface && value->type != type)
    return false;

  for(Property const& property : properties)
  {
    if(!property.validate(value))
      return false;
  }

  std::map<std::string, int> n;
  for(Child const& child : children)
  {
    n[child.getType()] = 0;
  }

  for(qmlon::Object::Reference object : value->children)
  {
    bool valid = false;
    for(Child const& child : children)
    {
      if(child.validate(object))
      {
        valid = true;
        n[child.getType()] += 1;

        if(child.getMax().set && child.getMax().value < n[child.getType()])
          return false;

        break;
      }
    }

    if(!valid)
      return false;
  }

  for(Child const& child : children)
  {
    if(child.getMin().set && child.getMin().value > n[child.getType()])
      return false;
  }

  return true;
}

bool qmlon::Schema::BooleanValue::validate(qmlon::Value::Reference const& value) const
{
  return value->isBoolean();
}

bool qmlon::Schema::IntegerValue::validate(qmlon::Value::Reference const& value) const
{
  if(!value->isInteger())
    return false;

  int i = value->asInteger();
  return (!min.set || i >= min.value) && (!max.set || i <= max.value);
}

bool qmlon::Schema::FloatValue::validate(qmlon::Value::Reference const& value) const
{
  if(!value->isFloat())
    return false;

  float f = value->asFloat();
  return (!min.set || f >= min.value) && (!max.set || f <= max.value);
}

bool qmlon::Schema::StringValue::validate(qmlon::Value::Reference const& value) const
{
  if(!value->isString())
    return false;

  std::string s = value->asString();
  return (!min.set || s.length() >= min.value) && (!max.set || s.length() <= max.value);
}

bool qmlon::Schema::ListValue::validate(qmlon::Value::Reference const& value) const
{
  if(!value->isList())
    return false;

  qmlon::Value::List l = value->asList();

  if((min.set && l.size() < min.value) || (max.set && l.size() > max.value))
    return false;

  for(qmlon::Value::Reference const& v : l)
  {
    bool valid = false;
    for(Value::Reference type : validTypes.value)
    {
      if(type->validate(v))
      {
        valid = true;
        break;
      }
    }

    if(!valid)
      return false;
  }

  return true;
}

bool qmlon::Schema::ObjectValue::validate(qmlon::Value::Reference const& value) const
{
  if(!value->isObject())
    return false;

  if(!type.set)
    return true;

  auto object = schema->getObjects().find(type.value);
  if(object == schema->getObjects().end())
    return false;

  return object->second.validate(value->asObject());
}

bool qmlon::Schema::validate(qmlon::Value::Reference const& value) const
{
  if(root.empty())
    return false;

  if(!value->isObject())
    return false;

  auto rootObject = objects.find(root);
  if(rootObject == objects.end())
    return false;

  return rootObject->second.validate(value->asObject());
}
