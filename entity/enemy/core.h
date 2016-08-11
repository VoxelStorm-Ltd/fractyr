#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include "enemy.h"

class buffer_enemy_core;                                                        // forward dec

class core : public enemy {
public:
  static buffer_enemy_core buf;                                                 // this object's graphics buffer
protected:
  float scale = 1.0;

public:
  core(world &parent_world,
       chunk *parent_chunk,
       vector3f const &position,
       quatf const &orientation = quatf::from_euler_angles(0.0, 0.0, 0.0),
       float scale = 1.0);
  ~core();

  void render() const override final;
  void update() override final;
  void destroy() override final;

  float get_collision_damage() const override;
  virtual entity::entity_type get_entity_type() const override;
};

#endif // CORE_H_INCLUDED
