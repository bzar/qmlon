#include "qmloninitializer.h"
#include <iostream>

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
    {"x", qmlon::set(&Frame::Position::x)},
    {"y", qmlon::set(&Frame::Position::y)}
  });

  qmlon::Initializer<Frame::Size> initSize({
    {"width", qmlon::set(&Frame::Size::width)},
    {"height", qmlon::set(&Frame::Size::height)}
  });

  qmlon::Initializer<Frame> initFrame({
    {"position", qmlon::createSet(initPosition, &Frame::setPosition)},
    {"hotspot", qmlon::createSet(initPosition, &Frame::setHotspot)},
    {"size", qmlon::createSet(initSize, &Frame::setSize)}
  });

  qmlon::Initializer<Animation> initAnimation({
    {"id", qmlon::set(&Animation::setId)}
  }, {
    {"Frame", qmlon::createAdd(initFrame, &Animation::addFrame)},
    {"Frames", [&](Animation& a, qmlon::Object& o) {
      int count = o.hasProperty("count") ? o.getProperty("count")->asInteger() : 1;
      int dx = 0;
      int dy = 0;

      if(o.hasProperty("delta"))
      {
        qmlon::Object& d = o.getProperty("delta")->asObject();
        dx = d.hasProperty("x") ? d.getProperty("x")->asInteger() : 0;
        dy = d.hasProperty("y") ? d.getProperty("y")->asInteger() : 0;
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
    {"id", qmlon::set(&Sprite::setId)}
  }, {
    {"Animation", qmlon::createAdd(initAnimation, &Sprite::addAnimation)}
  });

  qmlon::Initializer<SpriteSheet> initSheet({
    {"image", qmlon::set(&SpriteSheet::setImage)}
  }, {
    {"Sprite", qmlon::createAdd(initSprite, &SpriteSheet::addSprite)}
  });


  // Load QMLON description file
  qmlon::Value::Reference value = qmlon::readFile("spritesheet.qmlon");

  // Create root object and initialize tree
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
