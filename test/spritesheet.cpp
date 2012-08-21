#include "qmlon.h"
#include <iostream>
#include <fstream>

class Frame
{
public:
  Frame() : position(), hotspot(), size() {}
  struct Position
  {
    Position() : x(0), y(0) {}
    int x;
    int y;
  };

  struct Size
  {
    Size() : width(0), height(0) {}
    int width;
    int height;
  };

  Position getPosition() const { return position; }
  Position getHotspot() const { return hotspot; }
  Size getSize() const { return size; }

  void setPosition(Position const value) { position = value; }
  void setHotspot(Position const value) { hotspot = value; }
  void setSize(Size const value) { size = value; }

private:
  Position position;
  Position hotspot;
  Size size;
};

class Animation
{
public:
  Animation() : id(), frames() {}

  std::string const& getId() const { return id; }
  std::vector<Frame> const& getFrames() const { return frames; };

  void setId(std::string const& value) { id = value; }
  void addFrame(Frame const& value) { frames.push_back(value); }

private:
  std::string id;
  std::vector<Frame> frames;
};

class Sprite
{
public:
  Sprite() : id(), animations() {}

  std::string const& getId() const { return id; }
  std::map<std::string, Animation> const& getAnimations() const { return animations; };

  void setId(std::string const& value) { id = value; }
  void addAnimation(Animation const& value) { animations[value.getId()] = value; }

private:
  std::string id;
  std::map<std::string, Animation> animations;
};

class SpriteSheet
{
public:
  SpriteSheet() : image(), sprites() {}

  std::string const& getImage() const { return image; }
  std::map<std::string, Sprite> const& getSprites() const { return sprites; };

  void setImage(std::string const& value) { image = value; }
  void addSprite(Sprite const& value) { sprites[value.getId()] = value; }

private:
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
    {"position", [&](Frame& f, qmlon::Value::Reference v) { f.setPosition(qmlon::create(v, initPosition)); }},
    {"hotspot", [&](Frame& f, qmlon::Value::Reference v) { f.setHotspot(qmlon::create(v, initPosition)); }},
    {"size", [&](Frame& f, qmlon::Value::Reference v) { f.setSize(qmlon::create(v, initSize)); }}
  });

  qmlon::Initializer<Animation> initAnimation({
    {"id", [](Animation& a, qmlon::Value::Reference v) { a.setId(v->asString()); }}
  }, {
    {"Frame", [&](Animation& a, qmlon::Object* o) { a.addFrame(qmlon::create(o, initFrame)); }},
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
        Frame f = qmlon::create(o, initFrame);
        Frame::Position p = f.getPosition();
        p.x += i * dx;
        p.y += i * dy;
        f.setPosition(p);
        a.addFrame(f);
      }
    }}
  });

  qmlon::Initializer<Sprite> initSprite({
    {"id", [](Sprite& s, qmlon::Value::Reference v) { s.setId(v->asString()); }}
  }, {
    {"Animation", [&](Sprite& s, qmlon::Object* o) { s.addAnimation(qmlon::create(o, initAnimation)); }}
  });

  qmlon::Initializer<SpriteSheet> initSheet({
    {"image", [](SpriteSheet& sheet, qmlon::Value::Reference value) { sheet.setImage(value->asString()); }}
  }, {
    {"Sprite", [&](SpriteSheet& sheet, qmlon::Object* obj) { sheet.addSprite(qmlon::create(obj, initSprite)); }}
  });


  // Load QMLON description file
  std::ifstream f("spritesheet.qmlon");
  qmlon::Value::Reference value = qmlon::readValue(f);

  // Create root object and initialize tree (could also use qmlon::create)
  SpriteSheet sheet;
  initSheet.init(sheet, value);

  // Print results
  std::cout << "Sheet image: " << sheet.getImage() << std::endl;
  std::cout << "Number of sprites: " << sheet.getSprites().size() << std::endl;

  for(auto i : sheet.getSprites())
  {
    Sprite& s = i.second;
    std::cout << "  Sprite " << s.getId() << std::endl;

    for(auto j : s.getAnimations())
    {
      Animation& a = j.second;
      std::cout << "    Animation " << a.getId() << std::endl;

      for(auto f : a.getFrames())
      {
        std::cout << "      Frame" << std::endl;
        std::cout << "        position: (" << f.getPosition().x << ", " << f.getPosition().y << ")"<< std::endl;
        std::cout << "        hotspot:  (" << f.getHotspot().x << ", " << f.getHotspot().y << ")"<< std::endl;
        std::cout << "        size:      " << f.getSize().width << "x" << f.getSize().height << std::endl;
      }
    }
  }

  return EXIT_SUCCESS;
}
