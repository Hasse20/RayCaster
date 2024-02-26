#ifndef __i_world__
#define __i_world__

#include <memory>
#include <optional>

#include "map.hpp"

struct IWorld {
  using Ptr = std::shared_ptr<IWorld>;

  virtual std::optional<Collition> findCollition(const Point& position,
                                                 float angle) const = 0;

  // Try to move a certain distance in direction from a starting point. Return the destination point
  virtual Point move(const Point& from, const Direction& direction,
             float distance) const = 0;

  virtual ~IWorld() = default;
};

#endif  // !__i_world__
