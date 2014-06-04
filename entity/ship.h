#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "entity.h"
#include <vector>

class weapon;         // forward dec

class ship : public entity {
protected:
  float acceleration = 0.02;              // engine acceleration, m/frame^2
  std::vector<weapon*> weapons;           // what this ship is currently equipped with

public:
  ship(world &parent_world, chunk *parent_chunk,
       Vector3f const &position,
       Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
  virtual ~ship();

  Vector3f get_weapon_position();

  void update() override;

  void add_weapon(weapon *new_weapon);

  void accelerate(Vector3f accel);
  void rotate(float yaw, float pitch);
  void roll(float roll);
  void fire(unsigned int weapon_id);
};

#endif // SHIP_H_INCLUDED
