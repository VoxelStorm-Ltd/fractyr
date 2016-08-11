#include "particle.h"

buffer_shard particle::buf;

particle::particle(world &parent_world,
                   chunk *parent_chunk,
                   vector3f const &position,
                   quatf const &orientation,
                   vector3f const &new_velocity)
  : entity(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
  velocity = new_velocity;
  time_to_live *= (static_cast<float>(rand()) / RAND_MAX) + 0.5;
}

particle::~particle() {
  /// Default destructor
}

void particle::collided_with(entity *other __attribute__((unused))) {
  energy = 0;                                                                   // Destroy this particle.
}

entity::entity_type particle::get_entity_type() const {
  return entity::entity_type::BULLET;
}

void particle::update() {
  if(time_to_live == 0) {
    energy = 0;
    return;
  }
  entity::update();
  --time_to_live;
}

void particle::render() const {
  /// Draw this in the right place
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);
  glMultMatrixf(orientation_conjugate.transform());
  buf.render();
  glPopMatrix();
}

float particle::get_collision_damage() const {
  return 10;
}
