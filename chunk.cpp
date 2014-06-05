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
#include "grunt.h"
#include "blaster.h"

chunk::chunk(Vector3i const &chunk_coords, world &parent)
  : parent(&parent),
    coords(chunk_coords) {
  /// Default constructor

  // generate enemies
  grunt *grunt1 = new grunt(parent, this, Vector3f(70.0, 10.0, 50.0));
  grunt1->add_weapon(new blaster(grunt1, 120.0, 0.9));

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
  /// Return a guaranteed unique seed for this chunk
  return get_unique_seed(coords);
}

Vector3f chunk::check_collision(Vector3f const &coords, float radius) const {
  /// Check if a given point is colliding, and if so, return a normal vector to the collision surface
  // NOTE: coords can be less than 0 or greater than chunk::size by up to radius

  // TODO (note: use branch prediction hints for collision checks)

  return Vector3f(0.0, 0.0, 0.0);
}

void chunk::update() {
  /// Check for collisions and clean up any deleted entities in this chunk

  // Check for collisions between all pairs of entities in this chunk
  // TODO: Also check neighboring chunks.
  for(auto it = entities.begin(); it != entities.end(); ++it) {
    for(auto it2 = it+1; it2 != entities.end(); ++it2) {
      //std::cout << "Checking collision between " << *it << " and " << *it2 << std::endl;
      if((*it)->check_collision((*it2)->get_position(), (*it2)->radius).lengthSq() > 0) {
          (*it)->collided_with(*it2);
          (*it2)->collided_with(*it);
      }
    }
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
  std::vector<buffer_chunk::vertex> vbodata;
  std::vector<GLuint>               ibodata;

  // voronoi triangulation
  vbodata.reserve(20000);   // make sure to reserve the correct size to avoid re-allocations during construction
  ibodata.reserve(30000);

  // Create a non-periodic particle container
  float constexpr chunk_margin = 0.25;                                      // how far outside each chunk we compute the voronoi space to avoid discontinuities at edges
  //float constexpr chunk_margin = 1.0;                                       // how far outside each chunk we compute the voronoi space to avoid discontinuities at edges
  //float constexpr chunk_margin = -0.1;                                       // how far outside each chunk we compute the voronoi space to avoid discontinuities at edges
  voro::container con(-size * chunk_margin, size + (size * chunk_margin),   // the minimum and maximum x coordinates
                      -size * chunk_margin, size + (size * chunk_margin),   // the minimum and maximum y coordinates
                      -size * chunk_margin, size + (size * chunk_margin),   // the minimum and maximum z coordinates
                      6, 6, 6,                                              // the number of grid blocks in each of the three coordinate directions
                      false, false, false,                                  // flags setting whether the container is periodic in each coordinate direction - see http://math.lbl.gov/voro++/doc/refman/classvoro_1_1container.html#a50aaf382a069b102930b88976215818f
                      8);                                                   // the initial memory allocation for each block (number of particles)
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
        unsigned int const max_points = num_points + 600;
        for(int i = 0; i != 600; ++i) {
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
          con.put(num_points,
                  cell_point.x,
                  cell_point.y,
                  cell_point.z);
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
      Vector3d coords;
      cell_loop.pos(coords.x, coords.y, coords.z);
      if(coords.x < -size         * chunk_margin  ||
         coords.x >  size + (size * chunk_margin) ||
         coords.y < -size         * chunk_margin  ||
         coords.y >  size + (size * chunk_margin) ||
         coords.z < -size         * chunk_margin  ||
         coords.z >  size + (size * chunk_margin)) {
        continue;           // don't consider any cell that is outside of the test margin
      }
      std::vector<int> neighbours;              // list of neighbours IDs corresponding to faces  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l02198
      cell.neighbors(neighbours);
      bool solid = false;
      for(unsigned int face = 0; face != neighbours.size(); ++face) {
        //if(neighbours[face] < 0) {              // match external faces only (negative cell ID)
        //  if(coords.y > size * 0.25 &&
        //     coords.y < size * 0.75 &&
        //     coords.z > size * 0.25 &&
        //     coords.z < size * 0.75) {          // tunnel through the x wall
        //    solid = false;
        //  } else {
        //    solid = true;
        //  }
        //}
        // DEBUG ONLY:
        if(neighbours[face] < 0) {              // match external faces only (negative cell ID)
          solid = false;
        } else {
          if(coords.x > size * 0.2 &&
             coords.x < size * 0.8 &&
             coords.y > size * 0.2 &&
             coords.y < size * 0.8) {          // tunnel through the x wall
            solid = true;
          } else {
            solid = false;
          }
        }
      }
      cell_is_solid.insert(std::pair<int, bool>(cell_loop.pid(), solid));   // save the entry
    } else {
      std::cout << "WARNING: chunk " << coords << " failed to compute a cell" << std::endl;
    }
  } while(cell_loop.inc());

  cell_loop.start();                  // restart the loop

  // tesselate the appropriate cells
  do {
    if(con.compute_cell(cell, cell_loop)) {
      //int const cell_id = cell_loop.pid();
      // check whether to include the cell
      if(!cell_is_solid[cell_loop.pid()]) {
        continue;                               // skip this cell if it's an air cell
      }
      Vector3d coords;
      cell_loop.pos(coords.x, coords.y, coords.z);
      if(coords.x < 0.0  ||
         coords.x > size ||
         coords.y < 0.0  ||
         coords.y > size ||
         coords.z < 0.0  ||
         coords.z > size) {
        continue;           // don't render any cell that is outside of this chunk
      }

      // Gather information about the computed Voronoi cell
      std::vector<int>    neighbours;           // list of neighbours IDs corresponding to faces  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l02198
      std::vector<int>    face_verts;
      std::vector<double> verts;
      std::vector<double> normals;
      cell.neighbors(neighbours);
      cell.face_vertices(face_verts);           // 0-bracketed list of vertex ids   http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l01839
      cell.vertices(coords.x, coords.y, coords.z, verts);
      cell.normals(normals);                    // normals by face, in threes  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l01639

      for(unsigned int face = 0, vert_offset = 0; face != neighbours.size(); ++face) {    // loop over all faces of the Voronoi cell
        if(///neighbours[face] >= 0 &&             // internal faces only - container edges have negative IDs
           !cell_is_solid[neighbours[face]]) {  // only draw faces between solid and air cells
          //std::cout << "DEBUG: face " << face << " neighbours[face] " << neighbours[face] << " face_verts[vert_offset] " << face_verts[vert_offset] << " vert_offset " << vert_offset << std::endl;
          Vector3f facecolour(0.92, 0.95, 1.00);  // 2329 Kid Glove normalised to 1
          if(fmodf(coords.x, 10.0) < 1.0) {
            facecolour.assign(1.0, 1.0, 0.0);     // test gold chunks
          }
          // DEBUG ONLY:
          if(neighbours[face] < 0) {
            facecolour.assign(1.0, 0.0, 0.0);     // mark container-cut surfaces red
          }
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
}
