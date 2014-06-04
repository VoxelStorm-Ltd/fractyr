#include "plasma.h"
#include "buffer_plasma.h"
#include "world.h"
#include "chunk.h"

buffer_plasma plasma::buf;

plasma::plasma(world &parent_world,
               chunk *parent_chunk,
               Vector3f const &position,
               Quatf const &orientation,
               Vector3f const &ship_velocity,
               float shot_speed)
  : bullet(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
  radius = 0.2;

  velocity = Vector3f(0.0, 0.0, -shot_speed);
  velocity.rotate(orientation_conjugate);
  velocity += ship_velocity;
}

plasma::~plasma() {
  /// Default destructor
}

void plasma::update() {
  if(time_to_live == 0) {
    health = 0;
    return;
  }
  entity::update();
  --time_to_live;
}

void plasma::render() const {
  /// Draw this in the right place
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);
  glMultMatrixf(orientation_conjugate.transform());
  buf.render();
  glPopMatrix();
}

float plasma::get_collision_damage() const {
  return 50;
}
