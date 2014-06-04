#ifndef PLASMA_H_INCLUDED
#define PLASMA_H_INCLUDED

#include "bullet.h"

class buffer_plasma;

class plasma : public bullet {
public:
  static buffer_plasma buf;                       // this object's graphics buffer

  unsigned int time_to_live = 60;                 // how long to exist for, in frames

public:
  plasma(world &parent_world,
         chunk *parent_chunk,
         Vector3f const &position,
         Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0),
         Vector3f const &ship_velocity = Vector3f(0.0, 0.0, 0.0));
  ~plasma();

  void update() override final;
  void render() const override final;
};

#endif // PLASMA_H_INCLUDED
