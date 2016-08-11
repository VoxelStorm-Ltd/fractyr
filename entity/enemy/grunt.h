#ifndef GRUNT_H_INCLUDED
#define GRUNT_H_INCLUDED

#include "enemy.h"

class buffer_enemy_grunt;                                                       // forward dec

class grunt : public enemy {
public:
  static buffer_enemy_grunt buf;                                                // this object's graphics buffer
protected:
  float scale = 1.0;

public:
  grunt(world &parent_world,
        chunk *parent_chunk,
        vector3f const &position,
        quatf const &orientation = quatf::from_euler_angles(0.0, 0.0, 0.0),
        float scale = 1.0);
  ~grunt();

  void render() const override final;
  void update() override final;
  void destroy() override final;

  float get_collision_damage() const override;
};

#endif // GRUNT_H_INCLUDED
