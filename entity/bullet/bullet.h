#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include "entity/entity.h"

class bullet : public entity {
  /// Polymorphic base class for projectiles fired by weapons
protected:
  bullet(world &parent_world,
         chunk *parent_chunk,
         Vector3f const &position,
         Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
public:
  virtual ~bullet();
  virtual void collided_with(entity* other) override;
  virtual entity::entity_type get_entity_type() const override;
};

#endif // BULLET_H_INCLUDED
