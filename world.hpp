#ifndef __World__
#define __World__

#include <vector>
#include <optional>

#include "i_world.hpp"
#include "map.hpp"
#include "player.hpp"

class World : public IWorld {
 public:
  Map m_map{10, 10, mapData, mapData + 100};
  Player m_player;
  std::vector<Opponent> m_opps;

  std::optional<Collition> findCollition(const Point& position, float angle) const override;
  Point move(const Point& from, const Direction& direction, float distance) const override;
};

#endif  // !__World__
