#include "plasma.h"
#include "buffers/buffer_plasma.h"

buffer_plasma plasma::buf;

plasma::plasma(world &this_parent_world,
               chunk *parent_chunk,
               vector3f const &this_position,
               quatf const &this_orientation,
               vector3f const &ship_velocity,
               float shot_speed)
  : bullet(this_parent_world, parent_chunk, this_position, this_orientation) {
  /// Default constructor
  radius = 0.2f;

  velocity = vector3f(0.0f, 0.0f, -shot_speed);
  velocity.rotate(orientation_conjugate);
  velocity += ship_velocity;
}

plasma::~plasma() {
  /// Default destructor
}

void plasma::update() {
  if(time_to_live == 0) {
    energy = 0;
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
  return 100;
}
