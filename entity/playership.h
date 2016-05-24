#ifndef PLAYERSHIP_H
#define PLAYERSHIP_H

#include "entity/ship.h"


class playership final : public ship {
public:
  #ifndef NDEBUG
    bool invincible = false;
  #endif
  static unsigned int constexpr max_energy = 1000;
  unsigned int cores_destroyed = 0;

  playership(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
  virtual ~playership();
  void update() override;
  void accelerate(Vector3f const &accel) override final;
  bool fire(unsigned int weapon_id) override final;
  void collided_with(entity *other) override final;
  virtual entity::entity_type get_entity_type() const override final;
};

#endif // PLAYERSHIP_H
