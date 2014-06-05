#ifndef PLAYERSHIP_H
#define PLAYERSHIP_H

#include <ship.h>


class playership : public ship {
public:
  #ifndef NDEBUG
  bool invincible = false;
  #endif
  static unsigned int constexpr max_energy = 1000;

  playership(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
  virtual ~playership();
  void update() override;
  void accelerate(Vector3f accel) override;
  bool fire(unsigned int weapon_id) override;
  void collided_with(entity *other) override;
};

#endif // PLAYERSHIP_H
