#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "entity.h"

class ship : public entity {
protected:
  float move_speed = 1.0 / 60.0;  // camera movement speed, metres per frame
  float damping = 0.8;            // multiplier for movement damping
  float acceleration = move_speed * ((1.0 / damping) - 1.0);   // the required acceleration to reach our desired max speed with our set damping

  Quatf orientation_conjugate;    // conjugation of entity::orientation, cached

public:
  ship(world &parent_world, chunk *parent_chunk, Vector3f const &position);
  ~ship();

  void accelerate(Vector3f accel);
  void rotate(float yaw, float pitch);
  void roll(float roll);
};

#endif // SHIP_H_INCLUDED
