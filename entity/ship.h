#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "entity.h"

class ship : public entity {
protected:
  float acceleration = 0.02;      // engine acceleration, m/frame^2

  Quatf orientation_conjugate;    // conjugation of entity::orientation, cached

public:
  ship(world &parent_world, chunk *parent_chunk, Vector3f const &position);
  ~ship();

  void accelerate(Vector3f accel);
  void rotate(float yaw, float pitch);
  void roll(float roll);
};

#endif // SHIP_H_INCLUDED
