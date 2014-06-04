#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "entity.h"

class bullet : public entity {
  /// Polymorphic base class for projectiles fired by weapons
protected:
  bullet(world &parent_world,
         chunk *parent_chunk,
         Vector3f const &position,
         Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
public:
  virtual ~bullet();
};

#endif // BULLET_H_INCLUDED
