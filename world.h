#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include <vector>
#include "vmath.h"

class chunk;
class entity;

class world {
  /// Container class for chunks, objects and their relevant functions
public:
  static int constexpr size = 8;                            // how many chunks in each direction
  float const drag = 1.0;                                   // equal to 0.5 * mass-density of the fluid

private:
  std::vector<std::vector<std::vector<chunk*>>> chunks;     // 3D vector of chunk pointers, initially null
  std::vector<entity*> entities;                            // every entity that exists in the world
  std::vector<chunk*> visible_chunks;                       // List of chunks which are currently visible.

public:
  world();
  ~world();

  // chunk operations and queries
  static void correct_chunk_coords(Vector3i &chunk_coords);
  chunk *get_chunk(Vector3i const &chunk_coords);
  void find_visible_chunks(Vector3i const &chunk_coords, Quatf const &view_direction, int range);
  Vector3f check_collision(Vector3i const &chunk_coords, Vector3f const &coords, float radius);
  void delete_buffers();
  void setup_buffers();
  void preload_chunks();
  void clear_chunks();
  void clear_entities();

  // update
  void update();
  void add_entity(entity *thisentity);
  void remove_entity(entity *thisentity);

  // rendering
  void render(Vector3i const &chunk_coords, Quatf const &view_direction);
};

#endif // WORLD_H_INCLUDED
