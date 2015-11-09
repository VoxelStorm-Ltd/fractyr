#ifndef CHUNK_H_INCLUDED
#define CHUNK_H_INCLUDED

#include <vector>
#include <voro++.hh>
#include "vmath.h"
#include "buffers/buffer_chunk.h"
#include "world.h"

class world;          // forward dec
class entity;

class chunk {
  /// A renderable and collidable area of space
public:
  static float constexpr size = 800.0 / world::size;    // chunk size per side

  world *parent = nullptr;                // what world it belongs to
  Vector3i coords;                        // where this chunk is

private:
  std::vector<entity*> entities;          // everything that's currently in this chunk

  buffer_chunk buf;                       // this chunk's graphics buffer

  // Create a non-periodic particle container
  static float constexpr point_density = 0.0006;   // points per cubic metre; default 0.0006 = 600 per 100m^3
  //static float constexpr point_density = 0.000005;   // points per cubic metre; default 0.0006 = 600 per 100m^3
  //static float constexpr point_density = 0.001;   // points per cubic metre; default 0.0006 = 600 per 100m^3
  static unsigned int constexpr max_points = point_density * size * size * size;
  static float constexpr chunk_margin = 0.25;                               // how far outside each chunk we compute the voronoi space to avoid discontinuities at edges
  static unsigned int constexpr ideal_points_per_block = 8;                 // the optimal number of points per block in a container
  static unsigned int constexpr expected_points = max_points +              // roughly how many points we're generating - central cube
                                                  (max_points * 4 * chunk_margin) +                                 // cardinal neighbours
                                                  (max_points * 8 * chunk_margin * chunk_margin) +                  // cardinal corners
                                                  (max_points * 8 * chunk_margin * chunk_margin * chunk_margin);    // remote corners
  static unsigned int constexpr num_cells = std::cbrt(expected_points / ideal_points_per_block);
  voro::container con;


  #ifndef NDEBUG
    static double total_time_taken;                 // time to generate, in milliseconds, for average
    static unsigned int total_chunks_generated;     // number of chunks, to calculate average time per chunk
  #endif

public:
  chunk(Vector3i const &chunk_coords, world &parent);
  ~chunk();

  // query
  Vector3i const &get_coords() const;
  static unsigned int get_unique_seed(Vector3i const &chunk_coords);
  unsigned int        get_unique_seed() const;
  // generators
  static bool get_is_solid(  Vector3i const &chunk_coords, Vector3f const &local_coords);
  bool        get_is_solid(  Vector3f const &local_coords) const;
  static Vector3f get_colour(Vector3i const &chunk_coords, Vector3f const &local_coords);
  Vector3f        get_colour(Vector3f const &local_coords) const;

  // collisions
  Vector3f check_collision(Vector3f const &coords, float radius);

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
