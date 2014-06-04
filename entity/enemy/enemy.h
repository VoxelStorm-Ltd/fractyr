#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "ship.h"

class enemy : public ship {
  /// Polymorphic base class for all enemies
protected:
  enemy(world &parent_world,
        chunk *parent_chunk,
        Vector3f const &position,
        Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
public:
  virtual ~enemy();

  virtual void update() override;
};

#endif // ENEMY_H_INCLUDED
