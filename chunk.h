#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <vector>
#include "vmath.h"
#include "buffer_chunk.h"

class world;          // forward dec
class entity;

class chunk {
  /// A renderable and collidable area of space
public:
  static float constexpr size = 100.0;    // chunk size per side

  world *parent = nullptr;                // what world it belongs to
  Vector3i coords;                        // where this chunk is

private:
  std::vector<entity*> entities;          // everything that's currently in this chunk

  buffer_chunk buf;                       // this chunk's graphics buffer

public:
  chunk(Vector3i const &chunk_coords, world &parent);
  ~chunk();

  // collisions
  Vector3f check_collision(Vector3f const &coords, float radius) const;

  // update
  void update();
  void add_entity(entity *thisentity);
  void remove_entity(entity *thisentity);

  // drawing
  void render(Vector3i const &view_chunk_coords) const;
  void refresh();
  void setup();
};

#endif // CHUNK_H_INCLUDED
