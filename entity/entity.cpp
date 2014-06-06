#include "entity.h"
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "world.h"
#include "chunk.h"
#include "gameplayer.h"

extern gameplayer player;

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
Vector3f entity::get_offset(Vector3i const &start_chunk_coords,
                            Vector3f const &start_coords,
                            Vector3i const &end_chunk_coords,
                            Vector3f const &end_coords) {
  /// Calculate the relative coordinate difference in local coords between this entity and another location
  Vector3i chunk_offset(start_chunk_coords - end_chunk_coords);
  if(chunk_offset.x >  world::size / 2) chunk_offset.x -= world::size;
  if(chunk_offset.y >  world::size / 2) chunk_offset.y -= world::size;
  if(chunk_offset.z >  world::size / 2) chunk_offset.z -= world::size;
  if(chunk_offset.x < -world::size / 2) chunk_offset.x += world::size;
  if(chunk_offset.y < -world::size / 2) chunk_offset.y += world::size;
  if(chunk_offset.z < -world::size / 2) chunk_offset.z += world::size;
  return Vector3f(start_coords - end_coords) + (chunk_offset * chunk::size);
}
Vector3f entity::get_offset(Vector3i const &other_chunk_coords,
                            Vector3f const &other_coords) const {
  /// Wrapper function for the above
  return get_offset(parent->coords, position, other_chunk_coords, other_coords);
}
Vector3f entity::get_offset(entity const &other_entity) const {
  /// Wrapper function for the above
  return get_offset(other_entity.get_parent()->coords, other_entity.get_position());
}
Vector3f entity::get_world_position() const {
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

  //std::cout << "DEBUG: Entity attempting to move to: " << static_cast<Vector3i>(newposition) << std::endl;
  correct_point(newposition, newparent);

  std::vector<Vector3f> test_dirs = {
    Vector3f(Vector3f( radius, 0,      0)          * orientation),
    Vector3f(Vector3f(-radius, 0,      0)          * orientation),
    Vector3f(Vector3f(0,       radius, 0)          * orientation),
    Vector3f(Vector3f(0,      -radius, 0)          * orientation),
    Vector3f(Vector3f(0,               0, radius)  * orientation),
    Vector3f(Vector3f(0,               0, -radius) * orientation),
  };

  //std::cout << "DEBUG: Corrected to: " << static_cast<Vector3i>(newposition) << std::endl;

  Vector3f test_dir_sum = Vector3f(0, 0, 0);
  unsigned int test_points_collided = 0;

  for (auto test_dir : test_dirs) {
    Vector3f test_point(test_dir + newposition);
    chunk *test_parent = parent;
    correct_point(test_point, test_parent);

    Vector3f const &collision_normal(parent_world.check_collision(test_parent->coords, test_point, radius));
    if(__builtin_expect(collision_normal != Vector3f(0.0f, 0.0f, 0.0f), 0)) {     // branch prediction hint: unlikely (the usual case will be no collision)
      ++test_points_collided;
      test_dir_sum += test_dir;
    }
  }

  // reflect our velocity by the collision vector
  if (__builtin_expect(test_points_collided != 0 && test_dir_sum != Vector3f(0.0, 0.0, 0.0), 0)) { // If none or all test points are colliding, don't do a collision.
    Vector3f collision_normal = -(test_dir_sum/test_points_collided);
    collision_normal.normalise();
    #ifndef NDEBUG
    if(!player.noclip) {
      if (this->get_entity_type() == entity_type::PLAYER) {
        std::cout << "DEBUG: " << test_points_collided << ": " << test_dir_sum << " collision!  Normal " << collision_normal << std::endl;
      }
    #endif // NDEBUG
      velocity = direction - (collision_normal  * ((direction.dotProduct(collision_normal) * 2) / collision_normal.lengthSq()));
      velocity /= 2.0;
    #ifndef NDEBUG
    }
    #endif // NDEBUG

    //velocity = Vector3f(0.0f, 0.0f, 0.0f);
    // apply damping
    // TODO

    newposition = position + velocity;
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

void entity::move_force(Vector3f const &direction) {
  /// As normal move, but don't do any collision checking
  Vector3f newposition(position + direction);
  chunk *newparent = parent;
  correct_point(newposition, newparent);
  position = newposition;
  if(parent != newparent) {
    parent->remove_entity(this);
    newparent->add_entity(this);
    parent = newparent;
  }
}

void entity::correct_point(Vector3i &chunk_coords, Vector3f &coords) {
  /// Take a pair of speculative coords that may be out of bounds and return a valid pair of coords and chunk coords
  if(__builtin_expect(coords.x > chunk::size, 0)) {      // branch prediction hint: unlikely (the usual case will not be chunk changes)
    coords.x -= chunk::size;
    chunk_coords.x += 1;
    world::correct_chunk_coords(chunk_coords);
  }
  if(__builtin_expect(coords.x < 0.0f, 0)) {
    coords.x += chunk::size;
    chunk_coords.x -= 1;
    world::correct_chunk_coords(chunk_coords);
  }
  if(__builtin_expect(coords.y > chunk::size, 0)) {
    coords.y -= chunk::size;
    chunk_coords.y += 1;
    world::correct_chunk_coords(chunk_coords);
  }
  if(__builtin_expect(coords.y < 0.0f, 0)) {
    coords.y += chunk::size;
    chunk_coords.y -= 1;
    world::correct_chunk_coords(chunk_coords);
  }
  if(__builtin_expect(coords.z > chunk::size, 0)) {
    coords.z -= chunk::size;
    chunk_coords.z += 1;
    world::correct_chunk_coords(chunk_coords);
  }
  if(__builtin_expect(coords.z < 0.0f, 0)) {
    coords.z += chunk::size;
    chunk_coords.z -= 1;
    world::correct_chunk_coords(chunk_coords);
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
  if (get_entity_type() != entity::entity_type::ENEMY && other->get_entity_type() != entity::entity_type::ENEMY) {
    energy -= other->get_collision_damage();
  }
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
