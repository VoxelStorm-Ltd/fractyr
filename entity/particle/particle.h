#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

#include "entity/entity.h"
#include "buffers/buffer_shard.h"

class particle : public entity {
public:
  static buffer_shard buf;                                                      // this object's graphics buffer

  unsigned int time_to_live = 30;                                               // how long to exist for, in frames

public:
  particle(world &parent_world,
           chunk *parent_chunk,
           vector3f const &position,
           quatf const &orientation = quatf::from_euler_angles(0.0, 0.0, 0.0),
           vector3f const &velocity = vector3f(0.0, 0.0, 0.0));
  virtual ~particle();

  virtual void collided_with(entity *other) override;
  virtual entity::entity_type get_entity_type() const override;

  void update() override final;
  void render() const override final;

  float get_collision_damage() const override;
};

#endif // PARTICLE_H_INCLUDED
