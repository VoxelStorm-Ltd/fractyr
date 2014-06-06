#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "vmath.h"
#include "buffer.h"

class world;        // forward dec
class chunk;

class entity {
  /// Polymorphic base class for all positionally located entities

public:
  enum class entity_type : char {
    UNKNOWN,
    BULLET,
    ENEMY,
    PLAYER
  };
protected:
  world &parent_world;                    // what world we're in
  chunk *parent = nullptr;                // what chunk we're currently in
  Vector3f position;                      // coords within a chunk, metres
  Vector3f velocity;                      // where it's moving, metres per second
  Quatf orientation;                      // which way we're pointing
  Quatf orientation_conjugate;            // conjugation of entity::orientation, cached

  float mass   = 1.0;                     // how much this weighs, kilograms
  float drag   = 0.0;                     // how much drag this entity experiences (coefficient of drag * cross-sectional area)

public:
  float energy = 100.0;                   // general stat used for health/moving/firing.
  float radius = 0.0;                     // bounding sphere, metres

  entity(world &parent_world,
         chunk *parent_chunk,
         Vector3f const &position,
         Quatf const &orientation = Quatf::fromEulerAngles(0.0, 0.0, 0.0));
  virtual ~entity();

  world *get_parent_world() const;
  chunk *get_parent() const;
  Vector3f const &get_position() const;
  static Vector3f get_offset(Vector3i const &start_chunk_coords, Vector3f const &start_coords, Vector3i const &end_chunk_coords,  Vector3f const &end_coords);
  Vector3f get_offset(Vector3i const &other_chunk_coords, Vector3f const &other_coords) const;
  Vector3f get_offset(entity const &other_entity) const;
  Vector3f get_world_position() const;
  Vector3f const &get_velocity() const;
  Quatf const &get_orientation() const;
  Quatf const &get_orientation_conjugate() const;
  virtual entity_type get_entity_type() const;

  virtual void update();
  void move(Vector3f const &direction);
  void move_force(Vector3f const &direction);
  static void correct_point(Vector3i &chunk_coords, Vector3f &coords);
  static void correct_point(Vector3f &coords, chunk *&thischunk);

  Vector3f check_collision(Vector3f const &other_coords, float other_radius) const;
  virtual float get_collision_damage() const;
  virtual void collided_with(entity *other);

  virtual void render() const;
  virtual void render_from() const;
};

#endif // ENTITY_H_INCLUDED
