#include "map.hpp"


float degToRad(int a) { return a * M_PI / 180.0f; }

float fixAngle(float a) {
  if (a >= 360) {
    a -= 360;
  }
  if (a < 0) {
    a += 360;
  }
  return a;
}

// clang-format off
int mapData[] = {
    // the map array. Edit to change level but keep the outer walls
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 1, 1, 0, 1,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 0, 0, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 1, 0, 1,
    1, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
// clang-format on
