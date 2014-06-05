#include "chunk.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <voro++.hh>
#include "entity.h"
#include "universe.h"
#include "world.h"

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
  std::vector<buffer_chunk::vertex> vbodata;
  std::vector<GLuint>               ibodata;

  // voronoi triangulation
  vbodata.reserve(100);   // make sure to reserve the correct size to avoid re-allocations during construction
  ibodata.reserve(100);


  // Create a container with the geometry given above, and make it
  // non-periodic in each of the three coordinates. Allocate space for
  // eight particles within each computational block
  ///voro::container con(0.0, size,                   // the minimum and maximum x coordinates
  ///                    0.0, size,                   // the minimum and maximum y coordinates
  ///                    0.0, size,                   // the minimum and maximum z coordinates
  voro::container con(size * 0.25, size * 0.75,     // the minimum and maximum x coordinates
                      size * 0.25, size * 0.75,     // the minimum and maximum y coordinates
                      size * 0.25, size * 0.75,     // the minimum and maximum z coordinates
                      6, 6, 6,                      // the number of grid blocks in each of the three coordinate directions
                      false, false, false,          // flags setting whether the container is periodic in each coordinate direction - see http://math.lbl.gov/voro++/doc/refman/classvoro_1_1container.html#a50aaf382a069b102930b88976215818f
                      8);                           // the initial memory allocation for each block (number of particles)

  // Randomly add particles into the container
  for(unsigned int i = 0; i != 200; ++i) {
    Vector3f const coords(float(rand()) / RAND_MAX * size,
                          float(rand()) / RAND_MAX * size,
                          float(rand()) / RAND_MAX * size);
    con.put(i, coords.x, coords.y, coords.z);
  }

  std::cout << "DEBUG: Voronoi volume: " << con.sum_cell_volumes() << std::endl;

  voro::c_loop_all cell_loop(con);
  voro::voronoicell_neighbor cell;
  if(cell_loop.start()) {
    do {
      if(con.compute_cell(cell, cell_loop)) {
        Vector3d coords;
        cell_loop.pos(coords.x, coords.y, coords.z);
        int cell_id = cell_loop.pid();

        //cell.print_edges();      // DEBUG
        //std::cout << "DEBUG " << coords << std::endl;

        // Gather information about the computed Voronoi cell
        std::vector<int> neighbour;           // list of neighbour IDs corresponding to faces  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l02198
        cell.neighbors(neighbour);
        std::vector<int> face_verts;
        cell.face_vertices(face_verts);       // 0-bracketed list of vertex ids   http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l01839
        std::vector<double> normals;
        cell.normals(normals);                // normals by face, in threes  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l01639
        std::vector<double> verts;
        cell.vertices(coords.x, coords.y, coords.z, verts);
        // TODO: if this is unused, comment it out

        // Loop over all faces of the Voronoi cell
        for(unsigned int face = 0, j = 0; face != neighbour.size(); ++face) {
          // Skip if the neighbor information is smaller than
          // this particle's ID, to avoid double counting. This
          // also removes faces that touch the walls, since the
          // neighbor information is set to negative numbers for
          // these cases.
          ///if(neighbour[face] > cell_id) {
          ///if(neighbour[face] >= 0) {   // internal faces only
          if(neighbour[face] < 0) {       // external faces only
            // TODO: check if the two cells we're looking between are air/stone
            std::cout << "DEBUG: face " << face << " neighbour[face] " << neighbour[face] << " face_verts[j] " << face_verts[j] << " j " << j << std::endl;

            // generate a list of the coordinates of each vertex
            std::vector<Vector3f> vert_coords;
            std::vector<Vector3f> vert_normals;
            vert_coords.reserve( face_verts[j]);
            vert_normals.reserve(face_verts[j]);
            for(int i = 0; i < face_verts[j]; ++i) {
              int temp = 3 * face_verts[j + i + 1];
              vert_coords.emplace_back( verts[  temp], verts[  temp + 1], verts[  temp + 2]);
              vert_normals.emplace_back(normals[(face * 3)], normals[(face * 3) + 1], normals[face + 2]);
            }
            // tesselate this face
            for(int i = 2; i < face_verts[j]; ++i) {
              // counter-clockwise winding order
              unsigned int offset = vbodata.size();
              vbodata.emplace_back(vert_coords[0],     vert_normals[0]);
              vbodata.emplace_back(vert_coords[i],     vert_normals[i]);
              vbodata.emplace_back(vert_coords[i - 1], vert_normals[i - 1]);
              ibodata.emplace_back(offset + 0);
              ibodata.emplace_back(offset + 1);
              ibodata.emplace_back(offset + 2);
            }
          }
          // Skip to the next entry in the face vertex list
          j += face_verts[j] + 1;
        }
      }
    } while(cell_loop.inc());
  }

  // TODO:
  //voro::voronoicell_neighbor cell;
  //int num_edges = cell.number_of_edges();     // potentially slow function  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l02007
  //int num_faces = cell.number_of_faces();     // potentially slow function  http://math.lbl.gov/voro++/doc/refman/cell_8cc_source.html#l01721


  vbodata.shrink_to_fit();
  ibodata.shrink_to_fit();
  buf.setup(vbodata, ibodata);
}
