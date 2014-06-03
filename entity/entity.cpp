#include "entity.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "world.h"
#include "chunk.h"

entity::entity(world &parent_world, chunk *parent_chunk)
  : parent_world(parent_world),
    parent(parent_chunk),
    orientation(Quatd::fromEulerAngles(0.0, 0.0, 0.0)) {
  /// Default constructor
}

entity::~entity() {
  /// Default destructor
}

void entity::update() {
  move(velocity);

  // apply drag
  Vector3f const drag_force(velocity * velocity * drag * parent_world.drag);
  Vector3f const deceleration(drag_force / mass);
  velocity -= deceleration;
}

void entity::move(Vector3f const &direction) {
  Vector3f newposition(position + direction);
  chunk *newparent = parent;
  if(__builtin_expect(newposition.x > chunk::size, 0)) {      // branch prediction hint: unlikely (the usual case will not be chunk changes)
    newposition.x -= chunk::size;
    newparent = parent_world.get_chunk(parent->coords + Vector3i(1, 0, 0));
  }
  if(__builtin_expect(newposition.x < 0.0, 0)) {
    newposition.x += chunk::size;
    newparent = parent_world.get_chunk(parent->coords + Vector3i(-1, 0, 0));
  }
  if(__builtin_expect(newposition.y > chunk::size, 0)) {
    newposition.y -= chunk::size;
    newparent = parent_world.get_chunk(parent->coords + Vector3i(0, 1, 0));
  }
  if(__builtin_expect(newposition.y < 0.0, 0)) {
    newposition.y += chunk::size;
    newparent = parent_world.get_chunk(parent->coords + Vector3i(0, -1, 0));
  }
  if(__builtin_expect(newposition.z > chunk::size, 0)) {
    newposition.z -= chunk::size;
    newparent = parent_world.get_chunk(parent->coords + Vector3i(0, 0, 1));
  }
  if(__builtin_expect(newposition.z < 0.0, 0)) {
    newposition.z += chunk::size;
    newparent = parent_world.get_chunk(parent->coords + Vector3i(0, 0, -1));
  }

  Vector3f const &collision_vector(parent_world.check_collision(newparent->coords, newposition, radius));
  if(__builtin_expect(collision_vector != Vector3f(0.0, 0.0, 0.0), 0)) {     // branch prediction hint: unlikely (the usual case will be no collision)
    // reflect our velocity by the collision vector
    // TODO
    // apply damping
    // TODO
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

void entity::render() const {
  /// Render this entity
  // TODO
}

void entity::render_from() const {
  /// Render the world from this entity's point of view
  glPushMatrix();
  glMultMatrixf(orientation.transform());
  glTranslatef(position.x, position.y, position.z);

  parent_world.render(parent->coords, orientation);

  glPopMatrix();
}
