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

  bool cells_computed = false;
  struct cell {
    std::vector<int>    neighbours;       // list of neighbours IDs corresponding to faces
    std::vector<int>    face_verts;       // 0-bracketed list of vertex ids
    std::vector<double> verts;            // vertices per cell
    std::vector<double> normals;          // normals by face, in threes
  };

  #ifndef NDEBUG
    static double total_time_taken;                 // time to generate, in milliseconds, for average
    static unsigned int total_chunks_generated;     // number of chunks, to calculate average time per chunk
  #endif

public:
  chunk(Vector3i const &chunk_coords, world &parent);
  ~chunk();

  // query
  static unsigned int get_unique_seed(Vector3i const &chunk_coords);
  unsigned int get_unique_seed() const;
  // generators
  static bool get_is_solid(Vector3i const &chunk_coords, Vector3f const &local_coords);
  bool        get_is_solid(Vector3f const &local_coords) const;
  static Vector3f get_colour(Vector3i const &chunk_coords, Vector3f const &local_coords);
  Vector3f        get_colour(Vector3f const &local_coords) const;

  // collisions
  Vector3f check_collision(Vector3f const &coords, float radius) const;

  // update
  void update();
  void add_entity(entity *thisentity);
  void remove_entity(entity *thisentity);

  // drawing
  void render(Vector3i const &view_chunk_coords) const;
  void delete_buffers();
  void setup_buffers();
  void setup();
};

#endif // CHUNK_H_INCLUDED
