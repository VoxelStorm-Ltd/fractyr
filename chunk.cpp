#include "chunk.h"
#include <random>
#include <algorithm>
#include <map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <voro++.hh>
#include "entity.h"
#include "universe.h"
#include "world.h"

#ifndef NDEBUG
  double chunk::total_time_taken = 0;
  unsigned int chunk::total_chunks_generated = 0;
#endif

chunk::chunk(Vector3i const &chunk_coords, world &parent)
  : parent(&parent),
    coords(chunk_coords) {
  /// Default constructor
  setup_buffers();
}

chunk::~chunk() {
  /// Default destructor
  for(auto & e : entities) {
    delete e;
  }
}

unsigned int chunk::get_unique_seed(Vector3i const &chunk_coords) {
  /// Return a guaranteed unique seed for these chunk coordinates
  return (((chunk_coords.x * world::size) + chunk_coords.y) * world::size) + chunk_coords.z;
}
unsigned int chunk::get_unique_seed() const {
  /// Wrapper function for the static
  return get_unique_seed(coords);
}

bool chunk::get_is_solid(Vector3i const &chunk_coords, Vector3f const &local_coords) {
  /// Test a coordinate for solidity
  if(local_coords.x > size * 0.2 &&
     local_coords.x < size * 0.8 &&
     local_coords.y > size * 0.2 &&
     local_coords.y < size * 0.8) {               // solid tunnel through the x wall
    return true;
  } else {
    return false;
  }
}
bool chunk::get_is_solid(Vector3f const &local_coords) const {
  /// Wrapper function for the static
  return get_is_solid(coords, local_coords);
}

Vector3f chunk::get_colour(Vector3i const &chunk_coords, Vector3f const &local_coords) {
  /// Return the colour at these coordinates
  Vector3f facecolour(0.92, 0.95, 1.00);      // 2329 Kid Glove normalised to 1
  if(fmodf(local_coords.x, 10.0) < 1.0) {
    facecolour.assign(1.0, 1.0, 0.0);       // test gold chunks
  }
  return facecolour;
}
Vector3f chunk::get_colour(Vector3f const &local_coords) const {
  /// Wrapper function for the static
  return get_colour(coords, local_coords);
}

Vector3f chunk::check_collision(Vector3f const &coords, float radius) const {
  /// Check if a given point is colliding, and if so, return a normal vector to the collision surface
  // NOTE: coords can be less than 0 or greater than chunk::size by up to radius

  // TODO (note: use branch prediction hints for collision checks)

  return Vector3f(0.0, 0.0, 0.0);
}

void chunk::update() {
  /// Update every entity in this chunk
  for(auto & e : entities) {
    e->update();
  }
}

void chunk::add_entity(entity *thisentity) {
  /// Add this entity to our list
  entities.push_back(thisentity);
}

void chunk::remove_entity(entity *thisentity) {
  /// Take this entity out of our list
  //entities.erase(std::remove(entities.begin(), entities.end(), thisentity), entities.end());
  // should be faster:
  entities.erase(std::find(entities.begin(), entities.end(), thisentity));
  // TODO: for larger entity lists, instead sort and use std::binary_search
}

void chunk::render(Vector3i const &view_chunk_coords) const {
  /// Draw the contents of this chunk as viewed from coords
  Vector3i offset(coords - view_chunk_coords);
  int constexpr worldsize_half = world::size / 2;
  if(offset.x > worldsize_half) {
    offset.x -= world::size;
  }
  if(offset.y > worldsize_half) {
    offset.y -= world::size;
  }
  if(offset.z > worldsize_half) {
    offset.z -= world::size;
  }
  if(offset.x < -worldsize_half) {
    offset.x += world::size;
  }
  if(offset.y < -worldsize_half) {
    offset.y += world::size;
  }
  if(offset.z < -worldsize_half) {
    offset.z += world::size;
  }
  offset *= size;
  glPushMatrix();
  glTranslatef(offset.x, offset.y, offset.z);

  buf.render();

  for(auto & e : entities) {
    e->render();
  }

  glPopMatrix();
}

void chunk::delete_buffers() {
  /// Clean up the buffers for this chunk
  buf.destroy();
  buf.destroy_shader();
}

void chunk::setup_buffers() {
  /// Create the buffers for this chunk
  buf.init();
  buf.load_shader();
  setup();
}

void chunk::setup() {
  /// Refresh the buffer contents for this chunk
  #ifndef NDEBUG
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::duration<double>> timestart(std::chrono::high_resolution_clock::now());
  #endif

  std::vector<buffer_chunk::vertex> vbodata;
  std::vector<GLuint>               ibodata;

  // voronoi triangulation
  vbodata.reserve(20000);   // make sure to reserve the correct size to avoid re-allocations during construction
  ibodata.reserve(30000);

  // Create a non-periodic particle container
  float constexpr chunk_margin = 0.25;                                      // how far outside each chunk we compute the voronoi space to avoid discontinuities at edges
  unsigned int constexpr ideal_points_per_block = 8;                        // the optimal number of points per block in a container
  unsigned int constexpr expected_points = 2500;                            // roughly how many points we're generating; test and update this for release
  unsigned int constexpr num_cells = std::cbrt(expected_points / ideal_points_per_block);
  voro::container con(-size * chunk_margin, size + (size * chunk_margin),   // the minimum and maximum x coordinates
                      -size * chunk_margin, size + (size * chunk_margin),   // the minimum and maximum y coordinates
                      -size * chunk_margin, size + (size * chunk_margin),   // the minimum and maximum z coordinates
                      num_cells, num_cells, num_cells,                      // the number of grid blocks in each of the three coordinate directions
                      false, false, false,                                  // flags setting whether the container is periodic in each coordinate direction - see http://math.lbl.gov/voro++/doc/refman/classvoro_1_1container.html#a50aaf382a069b102930b88976215818f
                      ideal_points_per_block);                              // the initial memory allocation for each block (number of particles)
  // TODO: optimise this --^

  // populate this chunk and its neigbours' particles so we get a seamless join
  unsigned int num_points = 0;
  Vector3i offset;
  for(offset.x = -1; offset.x != 2; ++offset.x) {
    for(offset.y = -1; offset.y != 2; ++offset.y) {
      for(offset.z = -1; offset.z != 2; ++offset.z) {
        Vector3i chunk_coords(coords + offset);
        world::correct_chunk_coords(chunk_coords);      // wrap them if appropriate
        srand(get_unique_seed(chunk_coords));           // seed predictably by coords
        Vector3f const chunk_offset(offset * size);
        unsigned int constexpr max_points = 600;
        for(int i = 0; i != max_points; ++i) {
        //for(unsigned int i = 0; i != coords.y * 10; ++i) {
          Vector3f const cell_point((static_cast<float>(rand()) * size / RAND_MAX) + chunk_offset.x,
                                    (static_cast<float>(rand()) * size / RAND_MAX) + chunk_offset.y,
                                    (static_cast<float>(rand()) * size / RAND_MAX) + chunk_offset.z);
          if(cell_point.x < -size         * chunk_margin  ||
             cell_point.x >  size + (size * chunk_margin) ||
             cell_point.y < -size         * chunk_margin  ||
             cell_point.y >  size + (size * chunk_margin) ||
             cell_point.z < -size         * chunk_margin  ||
             cell_point.z >  size + (size * chunk_margin)) {
            continue;                   // don't add points outside the checked margin
          }
          con.put(num_points, cell_point.x, cell_point.y, cell_point.z);
          ++num_points;
        }
      }
    }
  }
  std::cout << "DEBUG: num_points " << num_points << std::endl;
  //std::cout << "DEBUG: Voronoi volume: " << con.sum_cell_volumes() << std::endl;

  std::map<int, bool> cell_is_solid;  // map of cell ID to whether they are solid or air

  voro::c_loop_all cell_loop(con);
  voro::voronoicell_neighbor cell;
  if(!cell_loop.start()) {            // start the loop
    std::cout << "WARNING: chunk " << coords << " contains zero voronoi cells" << std::endl;
    return;
  }
  // decide whether a cell is an air cell or a solid cell
  do {
    if(con.compute_cell(cell, cell_loop)) {
      Vector3d cell_coords;
      cell_loop.pos(cell_coords.x, cell_coords.y, cell_coords.z);
      if(cell_coords.x < -size         * chunk_margin  ||
         cell_coords.x >  size + (size * chunk_margin) ||
         cell_coords.y < -size         * chunk_margin  ||
         cell_coords.y >  size + (size * chunk_margin) ||
         cell_coords.z < -size         * chunk_margin  ||
         cell_coords.z >  size + (size * chunk_margin)) {
        continue;           // don't consider any cell that is outside of the test margin
      }
      bool solid;
      // skipping this check may be faster
      /*
      std::vector<int> neighbours;              // list of neighbours IDs corresponding to faces  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l02198
      cell.neighbors(neighbours);
      for(unsigned int face = 0; face != neighbours.size(); ++face) {
        if(neighbours[face] < 0) {              // match external faces only (negative cell ID)
          solid = false;
          goto skip_cell_check;
        }
      }
      */
      solid = get_is_solid(cell_coords);

      //skip_cell_check:
      cell_is_solid.insert(std::pair<int, bool>(cell_loop.pid(), solid));   // save the entry
    } else {
      ///std::cout << "WARNING: chunk " << coords << " failed to compute a cell" << std::endl;
    }
  } while(cell_loop.inc());

  cell_loop.start();                  // restart the loop

  // tesselate the appropriate cells
  do {
    // check whether to include the cell
    Vector3d cell_coords;
    cell_loop.pos(cell_coords.x, cell_coords.y, cell_coords.z);
    if(cell_coords.x < 0.0  ||
       cell_coords.x > size ||
       cell_coords.y < 0.0  ||
       cell_coords.y > size ||
       cell_coords.z < 0.0  ||
       cell_coords.z > size) {
      continue;           // don't render any cell that is outside of this chunk
    }
    //int const cell_id = cell_loop.pid();
    if(!cell_is_solid[cell_loop.pid()]) {
      continue;                               // skip this cell if it's an air cell
    }
    if(con.compute_cell(cell, cell_loop)) {
      // Gather information about the computed Voronoi cell
      std::vector<int>    neighbours;
      std::vector<int>    face_verts;
      std::vector<double> verts;
      std::vector<double> normals;
      cell.neighbors(neighbours);               // list of neighbours IDs corresponding to faces  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l02198
      cell.face_vertices(face_verts);           // 0-bracketed list of vertex ids   http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l01839
      cell.vertices(cell_coords.x, cell_coords.y, cell_coords.z, verts);
      cell.normals(normals);

      for(unsigned int face = 0, vert_offset = 0; face != neighbours.size(); ++face) {    // loop over all faces of the Voronoi cell
        if(//neighbours[face] >= 0 &&             // internal faces only - container edges have negative IDs
           !cell_is_solid[neighbours[face]]) {  // only draw faces between solid and air cells
          //std::cout << "DEBUG: face " << face << " neighbours[face] " << neighbours[face] << " face_verts[vert_offset] " << face_verts[vert_offset] << " vert_offset " << vert_offset << std::endl;
          #ifdef NDEBUG
            Vector3f const &facecolour(get_colour(cell_coords));
          #else
            Vector3f facecolour(get_colour(cell_coords));
            if(neighbours[face] < 0) {
              facecolour.assign(1.0, 0.0, 0.0);     // mark container-cut surfaces red
            }
          #endif
          // tesselation: generate a (clockwise) list of the coordinates and normals of each vertex for this face
          unsigned int offset = vbodata.size();
          for(int i = 0; i < face_verts[vert_offset]; ++i) {
            int vert_index = 3 * face_verts[vert_offset + i + 1];
            vbodata.emplace_back(Vector3f(verts[vert_index], verts[vert_index + 1], verts[vert_index + 2]),           // vertex coords
                                 Vector3f(normals[(face * 3)], normals[(face * 3) + 1], normals[(face * 3) + 2]),     // vertex normal
                                 facecolour);                                                                         // vertex colour:
          }
          // tesselation: add indices in counter-clockwise winding order
          for(int i = 2; i < face_verts[vert_offset]; ++i) {
            ibodata.emplace_back(offset + 0);
            ibodata.emplace_back(offset + i);
            ibodata.emplace_back(offset + i - 1);
          }
        }
        // Skip to the next entry in the face vertex list
        vert_offset += face_verts[vert_offset] + 1;
      }
    } else {
      std::cout << "WARNING: chunk " << coords << " failed to compute a cell" << std::endl;
    }
  } while(cell_loop.inc());

  vbodata.shrink_to_fit();
  ibodata.shrink_to_fit();
  buf.setup(vbodata, ibodata);

  #ifndef NDEBUG
    ++total_chunks_generated;
    double time_total = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timestart).count();
    total_time_taken += time_total;
    std::cout << "DEBUG: chunk took " << time_total << "ms, avg: " << total_time_taken / total_chunks_generated << "ms, total: " << total_time_taken << "ms" << std::endl;
  #endif
}
