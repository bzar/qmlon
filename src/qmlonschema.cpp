#include "qmlonschema.h"

qmlon::Schema& qmlon::Schema::initialize(Schema& schema, qmlon::Value::Reference value)
{
  qmlon::Initializer<BooleanValue> bvi;

  qmlon::Initializer<IntegerValue> ivi({
    {"min", [](IntegerValue& x, qmlon::Value::Reference v) { x.setMin(v->asInteger()); }},
    {"max", [](IntegerValue& x, qmlon::Value::Reference v) { x.setMax(v->asInteger()); }}
  });

  qmlon::Initializer<FloatValue> fvi({
    {"min", [](FloatValue& x, qmlon::Value::Reference v) { x.setMin(v->asFloat()); }},
    {"max", [](FloatValue& x, qmlon::Value::Reference v) { x.setMax(v->asFloat()); }}
  });

  qmlon::Initializer<StringValue> svi({
    {"min", [](StringValue& x, qmlon::Value::Reference v) { x.setMin(v->asInteger()); }},
    {"max", [](StringValue& x, qmlon::Value::Reference v) { x.setMax(v->asInteger()); }}
  });

  qmlon::Initializer<ObjectValue> ovi({
    {"type", [](ObjectValue& x, qmlon::Value::Reference v) { x.setType(v->asString()); }}
  });

  qmlon::Initializer<ListValue> lvi({
    {"min", [](ListValue& x, qmlon::Value::Reference v) { x.setMin(v->asInteger()); }},
    {"max", [](ListValue& x, qmlon::Value::Reference v) { x.setMax(v->asInteger()); }}
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
      ObjectValue* ov = new ObjectValue;
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
      throw std::runtime_error("ERROR: Not a Schema type");

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
    }},
  });

  qmlon::Initializer<Child> ci({
    {"min", [](Child& x, qmlon::Value::Reference v) { x.setMin(v->asInteger()); }},
    {"max", [](Child& x, qmlon::Value::Reference v) { x.setMax(v->asInteger()); }},
    {"type", [](Child& x, qmlon::Value::Reference v) { x.setType(v->asString()); }}
  });

  qmlon::Initializer<Object> oi({
    {"interface", [](Object& x, qmlon::Value::Reference v) { x.setIsInterface(v->asBoolean()); }},
    {"properties", [&](Object& x, qmlon::Value::Reference v) {
      qmlon::Value::List l = v->asList();
      for(qmlon::Value::Reference vr : l)
      {
        x.addProperty(qmlon::create(vr, pi));
      }
    }},
    {"children", [&](Object& x, qmlon::Value::Reference v) {
      qmlon::Value::List l = v->asList();
      for(qmlon::Value::Reference vr : l)
      {
        x.addChild(qmlon::create(vr, ci));
      }
    }}
  });

  qmlon::Initializer<Schema> si({
    {"root", [](Schema& x, qmlon::Value::Reference v) { x.setRoot(v->asString()); }}
  }, {
    {"", [&](Schema& x, qmlon::Object* obj) {
      Object o = qmlon::create(obj, oi);
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

bool qmlon::Schema::Object::validate(qmlon::Object const* value) const
{
  if(!isInterface && value->type != type)
    return false;


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

  return true;
}