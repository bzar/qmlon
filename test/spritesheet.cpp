#include "qmlon.h"
#include <iostream>

class Frame
{
public:
  struct Position
  {
    int x;
    int y;
  };

  struct Size
  {
    int width;
    int height;
  };

  Position position;
  Size size;
  Position hotspot;
};

class Animation
{
public:
  std::string id;
  std::vector<Frame> frames;
};

class Sprite
{
public:
  std::string id;
  std::map<std::string, Animation> animations;
};

class SpriteSheet
{
public:
  std::string image;
  std::map<std::string, Sprite> sprites;
};



int main(int argc, char** argv)
{
  // Create initializers for target object tree types

  qmlon::Initializer<Frame::Position> initPosition({
    {"x", [](Frame::Position& p, qmlon::Value::Reference v) { p.x = v->asInteger(); }},
    {"y", [](Frame::Position& p, qmlon::Value::Reference v) { p.y = v->asInteger(); }}
  });

  qmlon::Initializer<Frame::Size> initSize({
    {"width", [](Frame::Size& s, qmlon::Value::Reference v) { s.width = v->asInteger(); }},
    {"height", [](Frame::Size& s, qmlon::Value::Reference v) { s.height = v->asInteger(); }}
  });

  qmlon::Initializer<Frame> initFrame({
    {"position", [&](Frame& f, qmlon::Value::Reference v) { initPosition.init(f.position, v); }},
    {"hotspot", [&](Frame& f, qmlon::Value::Reference v) { initPosition.init(f.hotspot, v); }},
    {"size", [&](Frame& f, qmlon::Value::Reference v) { initSize.init(f.size, v); }}
  });

  qmlon::Initializer<Animation> initAnimation({
    {"id", [](Animation& a, qmlon::Value::Reference v) { a.id = v->asString(); }}
  }, {
    {"Frame", [&](Animation& a, qmlon::Object* o) {
      Frame f;
      initFrame.init(f, o);
      a.frames.push_back(f);
    }},
    {"Frames", [&](Animation& a, qmlon::Object* o) {
      int count = o->hasProperty("count") ? o->getProperty("count")->asInteger() : 1;
      int dx = 0;
      int dy = 0;

      if(o->hasProperty("delta"))
      {
        qmlon::Object* d = o->getProperty("delta")->asObject();
        dx = d->hasProperty("x") ? d->getProperty("x")->asInteger() : 0;
        dy = d->hasProperty("y") ? d->getProperty("y")->asInteger() : 0;
      }

      for(int i = 0; i < count; ++i)
      {
        Frame f;
        initFrame.init(f, o);
        f.position.x += i * dx;
        f.position.y += i * dy;
        a.frames.push_back(f);
      }
    }}
  });

  qmlon::Initializer<Sprite> initSprite({
    {"id", [](Sprite& s, qmlon::Value::Reference v) { s.id = v->asString(); }}
  }, {
    {"Animation", [&](Sprite& s, qmlon::Object* o) {
      Animation a;
      initAnimation.init(a, o);
      s.animations[a.id] = a;
    }}
  });

  qmlon::Initializer<SpriteSheet> initSheet({
    {"image", [](SpriteSheet& sheet, qmlon::Value::Reference value) { sheet.image = value->asString(); }}
  }, {
    {"Sprite", [&](SpriteSheet& sheet, qmlon::Object* obj) {
      Sprite sprite;
      initSprite.init(sprite, obj);
      sheet.sprites[sprite.id] = sprite;
    }}
  });


  // Load QMLON description file
  std::ifstream f("sprite.sheet");
  qmlon::Value::Reference value = qmlon::readValue(f);

  // Create root object and initialize tree
  SpriteSheet sheet;
  initSheet.init(sheet, value);

  // Print results
  std::cout << "Sheet image: " << sheet.image << std::endl;
  std::cout << "Number of sprites: " << sheet.sprites.size() << std::endl;

  for(auto i : sheet.sprites)
  {
    Sprite& s = i.second;
    std::cout << "  Sprite " << s.id << std::endl;

    for(auto j : s.animations)
    {
      Animation& a = j.second;
      std::cout << "    Animation " << a.id << std::endl;

      for(auto f : a.frames)
      {
        std::cout << "      Frame" << std::endl;
        std::cout << "        position: (" << f.position.x << ", " << f.position.y << ")"<< std::endl;
        std::cout << "        hotspot:  (" << f.hotspot.x << ", " << f.hotspot.y << ")"<< std::endl;
        std::cout << "        size:      " << f.size.width << "x" << f.size.height << std::endl;
      }
    }
  }

  return EXIT_SUCCESS;
}
