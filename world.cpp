#include "world.hpp"

using namespace std;

  optional<Collition> World::findCollition(const Point& position,
                                         float angle) const {
  optional<Collition> horizontalCollition{};
  optional<Collition> verticalCollition{};

  float collitionX, collitionY, stepX, stepY;

  // Find vertical hit
  int dof = 0;
  float distance = 100000;
  float tanAngle = tan(degToRad(angle));
  if (cos(degToRad(angle)) > 0.001) {  // Looking right
    collitionX = (((int)position.x >> 6) << 6) + 64;
    collitionY = (position.x - collitionX) * tanAngle + position.y;
    stepX = 64;
    stepY = -stepX * tanAngle;
  } else if (cos(degToRad(angle)) < -0.001) {  // Looking left
    collitionX = (((int)position.x >> 6) << 6) - 0.0001;
    collitionY = (position.x - collitionX) * tanAngle + position.y;
    stepX = -64;
    stepY = -stepX * tanAngle;
  } else {  // Looking up or down, no hit of vertical lines
    dof = 8;
  }

  while (dof < 8) {
    int mapX = (int)(collitionX) >> 6;
    int mapY = (int)(collitionY) >> 6;
    int mapObject = m_map.get(mapX, mapY);
    if (mapObject == 1) {  // It's a hit
      Point hit{collitionX, collitionY};
      distance = position.distance(hit);
      verticalCollition = {
          {collitionX, collitionY, distance, Orientation::Vertical}};
      break;
    } else {  // Check next horizontal
      collitionX += stepX;
      collitionY += stepY;
      dof += 1;
    }
  }

  // Find horizontal hit
  dof = 0;
  distance = 100000;
  tanAngle = 1.0 / tanAngle;
  if (sin(degToRad(angle)) > 0.001) {  // Looking up
    collitionY = (((int)position.y >> 6) << 6) - 0.0001;
    collitionX = (position.y - collitionY) * tanAngle + position.x;
    stepY = -64;
    stepX = -stepY * tanAngle;
  } else if (sin(degToRad(angle)) < -0.001) {  // Looking down
    collitionY = (((int)position.y >> 6) << 6) + 64;
    collitionX = (position.y - collitionY) * tanAngle + position.x;
    stepY = 64;
    stepX = -stepY * tanAngle;
  } else {  // looking straight left or right
    collitionX = position.x;
    collitionY = position.y;
    dof = 8;
  }

  while (dof < 8) {
    int mapX = (int)(collitionX) >> 6;
    int mapY = (int)(collitionY) >> 6;
    int mapObject = m_map.get(mapX, mapY);
    if (mapObject == 1) {  // It's a hit
      Point hit{collitionX, collitionY};
      distance = position.distance(hit);
      horizontalCollition = {
          {collitionX, collitionY, distance, Orientation::Horizontal}};
      break;
    } else {  // Check next horizontal
      collitionX += stepX;
      collitionY += stepY;
      dof += 1;
    }
  }

  optional<Collition> collition{};
  if (horizontalCollition &&
      verticalCollition) {  // Both horizontal and vertical hits, get the
                            // closest one
    collition = (horizontalCollition->distance < verticalCollition->distance)
                    ? horizontalCollition
                    : verticalCollition;
  } else if (horizontalCollition && !verticalCollition) {
    collition = horizontalCollition;
  } else if (!horizontalCollition && verticalCollition) {
    collition = verticalCollition;
  }

  return collition;
}

Point World::move(const Point& from, const Direction& direction, float distance) const {
  auto collition = findCollition(from, direction.angle);
  if (collition && collition->distance < distance + 1) {
    distance = collition->distance - 1;
  }

  return {from.x + direction.x * distance, from.y + direction.y * distance};
}
