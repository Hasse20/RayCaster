#ifndef __Player__
#define __Player__

#include <cstdlib>

#include "map.hpp"
#include "i_world.hpp"

class Player {
 public:
  Point position;
  Direction direction{90};

  void init() {
    position = {150, 520};
    direction = Direction(90);
  }

  void moveForward(IWorld& world) {
    position = world.move(position, direction, 5);
  }

  void moveBackward(IWorld& world) {
    Direction backward{fixAngle(direction.angle + 180)};
    position = world.move(position, backward, 5);

  }

  void turnLeft() {
    direction.set(fixAngle(direction.angle + 5));
  }

  void turnRight() { 
    direction.set(fixAngle(direction.angle - 5));
  }
};

// Future project, add opponents
class Opponent {};



#endif  // !__Player__
