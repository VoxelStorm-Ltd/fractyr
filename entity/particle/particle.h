#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

#include "entity.h"
#include "buffer_shard.h"

class particle : public entity {
public:
  static buffer_shard buf;                        // this object's graphics buffer

  unsigned int time_to_live = 30;                 // how long to exist for, in frames

public:
  particle(world &parent_world,
           chunk *parent_chunk,
           Vector3f const &position,
           Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0),
           Vector3f const &velocity = Vector3f(0.0, 0.0, 0.0));
  virtual ~particle();

  virtual void collided_with(entity* other) override;
  virtual entity::entity_type get_entity_type() const override;

  void update() override final;
  void render() const override final;

  float get_collision_damage() const override;
};

#endif // PARTICLE_H_INCLUDED
