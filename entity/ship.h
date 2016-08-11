#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "entity.h"
#include <vector>

class weapon;                                                                   // forward dec

class ship : public entity {
protected:
  float acceleration = 0.02;                                                    // engine acceleration, m/frame^2
  std::vector<weapon*> weapons;                                                 // what this ship is currently equipped with

public:
  ship(world &parent_world, chunk *parent_chunk,
       vector3f const &position,
       quatf const &orientation = quatf::from_euler_angles(0.0, 0.0, 0.0));
  virtual ~ship();

  vector3f get_weapon_position();

  void update() override;

  void add_weapon(weapon *new_weapon);

  virtual void accelerate(vector3f const &accel);
  void rotate(float yaw, float pitch);
  void roll(float roll);
  virtual bool fire(unsigned int weapon_id);                                    // Attempts to fire current weapon, returns whether successful or not.
};

#endif // SHIP_H_INCLUDED
