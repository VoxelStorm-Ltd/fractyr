#include "entity.h"
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "world.h"
#include "chunk.h"

entity::entity(world &parent_world,
               chunk *parent_chunk,
               Vector3f const &position,
               Quatf const &orientation)
  : parent_world(parent_world),
    parent(parent_chunk),
    position(position),
    orientation(orientation),
    orientation_conjugate(orientation) {
  /// Default constructor
  parent_world.add_entity(this);
  if(parent_chunk) {
    parent_chunk->add_entity(this);
  }
  orientation_conjugate.conjugate();
}

entity::~entity() {
  /// Default destructor
}

world *entity::get_parent_world() const {
  return &parent_world;
}
chunk *entity::get_parent() const {
  return parent;
}
Vector3f const &entity::get_position() const {
  return position;
}
Vector3f const entity::get_world_position() const {
  return position + (parent->coords * chunk::size);
}
Vector3f const &entity::get_velocity() const {
  return velocity;
}
Quatf const &entity::get_orientation() const {
  return orientation;
}
Quatf const &entity::get_orientation_conjugate() const {
  return orientation_conjugate;
}

entity::entity_type entity::get_entity_type() const {
  return entity::entity_type::UNKNOWN;
}

void entity::update() {
  move(velocity);

  // apply drag
  float const drag_force(velocity.lengthSq() * drag * parent_world.drag);
  float const deceleration(drag_force / mass);
  //std::cout << "DEBUG V = " << velocity.length() * 60.0 << "m/s, F = " << drag_force << "N, acc = " << deceleration << "m/s^2" << std::endl;
  if(velocity != Vector3f(0.0, 0.0, 0.0)) {
    velocity -= velocity.normalise_copy() * deceleration;
  }
}

void entity::move(Vector3f const &direction) {
  Vector3f newposition(position + direction);
  chunk *newparent = parent;

  correct_point(newposition, newparent);

  Vector3f const &collision_vector(parent_world.check_collision(newparent->coords, newposition, radius));
  if(__builtin_expect(collision_vector != Vector3f(0.0f, 0.0f, 0.0f), 0)) {     // branch prediction hint: unlikely (the usual case will be no collision)
    // reflect our velocity by the collision vector
    // TODO
    // apply damping
    // TODO

    correct_point(newposition, newparent);
  }

  position = newposition;
  if(parent != newparent) {
    parent->remove_entity(this);
    newparent->add_entity(this);
    //std::cout << "DEBUG: entity moved chunks from " << parent->coords << " to " << newparent->coords << std::endl;
    parent = newparent;
  }
}

void entity::correct_point(Vector3f &coords, chunk *&thischunk) {
  /// Take a coord in a chunk, that may be out of bounds, and return a valid pair of coords and chunk coords
  if(__builtin_expect(coords.x > chunk::size, 0)) {      // branch prediction hint: unlikely (the usual case will not be chunk changes)
    coords.x -= chunk::size;
    thischunk = thischunk->parent->get_chunk(thischunk->coords + Vector3i(1, 0, 0));
  }
  if(__builtin_expect(coords.x < 0.0f, 0)) {
    coords.x += chunk::size;
    thischunk = thischunk->parent->get_chunk(thischunk->coords + Vector3i(-1, 0, 0));
  }
  if(__builtin_expect(coords.y > chunk::size, 0)) {
    coords.y -= chunk::size;
    thischunk = thischunk->parent->get_chunk(thischunk->coords + Vector3i(0, 1, 0));
  }
  if(__builtin_expect(coords.y < 0.0f, 0)) {
    coords.y += chunk::size;
    thischunk = thischunk->parent->get_chunk(thischunk->coords + Vector3i(0, -1, 0));
  }
  if(__builtin_expect(coords.z > chunk::size, 0)) {
    coords.z -= chunk::size;
    thischunk = thischunk->parent->get_chunk(thischunk->coords + Vector3i(0, 0, 1));
  }
  if(__builtin_expect(coords.z < 0.0f, 0)) {
    coords.z += chunk::size;
    thischunk = thischunk->parent->get_chunk(thischunk->coords + Vector3i(0, 0, -1));
  }
}

Vector3f entity::check_collision(Vector3f const &other_coords, float other_radius) const {
  /// Check if the point is colliding with this entity, and if so, return the surface normal vector
  Vector3f vec(other_coords - position);
  float const distance = radius + other_radius;
  if(vec.lengthSq() < distance * distance) {      // using squared functions to avoid a square root
    vec.normalise();
    return vec;
  } else {
    return Vector3f(0.0, 0.0, 0.0);
  }
}

float entity::get_collision_damage() const {
  return 0.0;
}

void entity::collided_with(entity *other) {
  health -= other->get_collision_damage();
}

void entity::render() const {
  /// Render this entity
  // polymorphic placeholder
}

void entity::render_from() const {
  /// Render the world from this entity's point of view
  glPushMatrix();
  glMultMatrixf(orientation.transform());
  glTranslatef(-position.x,
               -position.y,
               -position.z);

  parent_world.render(parent->coords, orientation_conjugate);

  glPopMatrix();
}
