#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "vectorstorm/vectorstorm.h"
#include "buffers/buffer.h"

class world;                                                                    // forward dec
class chunk;

class entity {
  /// Polymorphic base class for all positionally located entities

public:
  enum class entity_type : char {
    UNKNOWN,
    BULLET,
    ENEMY,
    PLAYER,
    CORE
  };
protected:
  world &parent_world;                                                          // what world we're in
  chunk *parent = nullptr;                                                      // what chunk we're currently in
  vector3f position;                                                            // coords within a chunk, metres
  vector3f velocity;                                                            // where it's moving, metres per second
  quatf orientation;                                                            // which way we're pointing
  quatf orientation_conjugate;                                                  // conjugation of entity::orientation, cached

  float mass   = 1.0;                                                           // how much this weighs, kilograms
  float drag   = 0.0;                                                           // how much drag this entity experiences (coefficient of drag * cross-sectional area)

public:
  float energy = 100.0;                                                         // general stat used for health/moving/firing.
  float radius = 0.0;                                                           // bounding sphere, metres

  entity(world &parent_world,
         chunk *parent_chunk,
         vector3f const &position,
         quatf const &orientation = quatf::from_euler_angles(0.0, 0.0, 0.0));
  virtual ~entity();

  world *get_parent_world() const;
  chunk *get_parent() const;
  vector3f const &get_position() const;
  static vector3f get_offset(vector3i const &start_chunk_coords, vector3f const &start_coords, vector3i const &end_chunk_coords,  vector3f const &end_coords);
  vector3f get_offset(vector3i const &other_chunk_coords, vector3f const &other_coords) const;
  vector3f get_offset(entity const &other_entity) const;
  vector3f get_world_position() const;
  vector3f const &get_velocity() const;
  quatf const &get_orientation() const;
  quatf const &get_orientation_conjugate() const;
  virtual entity_type get_entity_type() const;

  virtual void update();
  void move(vector3f const &direction);
  void move_force(vector3f const &direction);
  static void correct_point(vector3i &chunk_coords, vector3f &coords);
  static void correct_point(vector3f &coords, chunk *&thischunk);
  virtual void destroy();

  vector3f check_collision(vector3f const &other_coords, float other_radius) const;
  virtual float get_collision_damage() const;
  virtual void collided_with(entity *other);

  virtual void render() const;
  virtual void render_from() const;
};

#endif // ENTITY_H_INCLUDED
