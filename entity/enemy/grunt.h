#ifndef GRUNT_H_INCLUDED
#define GRUNT_H_INCLUDED

#include "enemy.h"

class buffer_enemy_grunt;      // forward dec

class grunt : public enemy {
public:
  static buffer_enemy_grunt buf;                  // this object's graphics buffer
protected:
  float health = 10.0;                            // how much of a beating it can take

public:
  grunt(world &parent_world,
        chunk *parent_chunk,
        Vector3f const &position,
        Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
  ~grunt();

  void render() const override final;
  void update() override final;
};

#endif // GRUNT_H_INCLUDED
