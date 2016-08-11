#ifndef PLASMA_H_INCLUDED
#define PLASMA_H_INCLUDED

#include "bullet.h"

class buffer_plasma;                                                            // forward dec

class plasma : public bullet {
public:
  static buffer_plasma buf;                                                     // this object's graphics buffer

  unsigned int time_to_live = 180;                                              // how long to exist for, in frames

public:
  plasma(world &parent_world,
         chunk *parent_chunk,
         vector3f const &position,
         quatf const &orientation = quatf::from_euler_angles(0.0, 0.0, 0.0),
         vector3f const &ship_velocity = vector3f(0.0, 0.0, 0.0),
         float shot_speed = 1.5);
  ~plasma();

  void update() override final;
  void render() const override final;

  float get_collision_damage() const override;
};

#endif // PLASMA_H_INCLUDED
