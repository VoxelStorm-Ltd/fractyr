#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "vmath.h"

class world;        // forward dec
class chunk;

class entity {
  /// Polymorphic base class for all positionally located entities
protected:
  world &parent_world;                    // what world we're in
  chunk *parent = nullptr;                // what chunk we're currently in
  Vector3f position;                      // coords within a chunk, metres
  Vector3f velocity;                      // where it's moving, metres per second
  Quatf orientation;                      // which way we're pointing

  float mass   = 1.0;                     // how much this weighs, kilograms
  float radius = 0.0;                     // bounding sphere, metres
  float drag   = 0.0;                     // how much drag this entity experiences (coefficient of drag * cross-sectional area)

public:
  entity(world &parent_world, chunk *parent_chunk, Vector3f const &position);
  virtual ~entity();

  void update();
  void move(Vector3f const &direction);
  static void correct_point(Vector3f &coords, chunk *&thischunk);
  Vector3f check_collision(Vector3f const &other_coords, float other_radius) const;

  void render() const;
  void render_from() const;
};

#endif // ENTITY_H_INCLUDED
