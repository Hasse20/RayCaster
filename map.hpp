#ifndef __Map__
#define __Map__

#include <cmath>
#include <iostream>
#include <vector>

constexpr float M_PI = 3.141592653589F;

float degToRad(int a);

float fixAngle(float a);

int mapData[];

class Point {
 public:
  float x;
  float y;

  // Pytagoras theorem for calculating distance between two points
  float distance(const Point& p) const {
    const float dx = p.x - x;
    const float dy = p.y - y;
    return sqrt(dx * dx + dy * dy);
  }
};

class Direction {
 public:
  float x;
  float y;
  float angle;

  void set(float angle) { 
    this->angle = angle; 
    x = cos(degToRad(angle));
    y = -sin(degToRad(angle));

  }
  Direction() = delete;
  Direction(float angle) { set(angle); }
};

class Map {
 public:
  int m_width;
  int m_height;
  std::vector<int> m_mapData;

  template <class InputIt>
  Map(int width, int height, InputIt first, InputIt last)
      : m_width{width}, m_height{height}, m_mapData{first, last} {
    if (m_mapData.size() != m_width * m_height) {
      std::cerr << "Mapdata has wrong size" << std::endl;
      exit(-1);
    }
  };

  int get(int x, int y) const {
    if (x < 0 || m_width <= x || y < 0 || m_height <= y) {
      return -1;  // No map data
    }

    const int index = y * m_width + x;
    return m_mapData[index];
  }
};

enum class Orientation : int { Horizontal, Vertical };

struct Collition {
  Point position;
  float distance;
  Orientation orientation;
};


#endif  // !__Map__
