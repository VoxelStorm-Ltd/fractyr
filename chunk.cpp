#include "chunk.h"
#include <random>
#include <algorithm>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "entity.h"
#include "universe.h"

chunk::chunk(Vector3i const &chunk_coords, world &parent)
  : parent(&parent),
    coords(chunk_coords) {
  /// Default constructor
  setup_buffers();
  setup();
}

chunk::~chunk() {
  /// Default destructor
  for(auto &e : entities) {
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
  for(auto &e : entities) {
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
  Vector3i offset((coords - view_chunk_coords) * size);
  glPushMatrix();
  glTranslatef(-offset.x, -offset.y, -offset.z);

  buf.render();

  for(auto &e : entities) {
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

  // menger sponge
  unsigned int constexpr iters = 2;
  unsigned int constexpr numblocks = static_cast<int>(pow(3.0, static_cast<double>(iters)));
  float constexpr blocksize = size / numblocks;

  vbodata.reserve(6 * 4 * numblocks * numblocks * numblocks);   // make sure to reserve the correct size to avoid re-allocations during construction
  ibodata.reserve(6 * 6 * numblocks * numblocks * numblocks);

  for(unsigned int x = 0; x != numblocks; ++x) {
    for(unsigned int y = 0; y != numblocks; ++y) {
      for(unsigned int z = 0; z != numblocks; ++z) {
        //blocksize = static_cast<int>(pow(3.0, static_cast<double>(iters))) * x/static_cast<float>(numblocks) * y/static_cast<float>(numblocks) * z/static_cast<float>(numblocks);
        bool skip = false;
        int depth = 1;
        for(unsigned int i = 0; i != iters; ++i) {
          int matches = 0;
          if((x / depth) % 3 == 1) {
            ++matches;
          }
          if((y / depth) % 3 == 1) {
            ++matches;
          }
          if((z / depth) % 3 == 1) {
            ++matches;
          }
          if(matches >= 2) {
            skip = true;
            break;
          }
          depth *= 3;
        }
        if(skip) {
          continue;
        }
        Vector3f const coord000(blocksize * x, blocksize * y, blocksize * z);
        Vector3f const coord100(coord000 + Vector3f(blocksize, 0.0f, 0.0f));
        Vector3f const coord010(coord000 + Vector3f(0.0f, blocksize, 0.0f));
        Vector3f const coord110(coord000 + Vector3f(blocksize, blocksize, 0.0f));

        Vector3f const coord001(blocksize * x, blocksize * y, blocksize * z + blocksize);
        Vector3f const coord101(coord001 + Vector3f(blocksize, 0.0f, 0.0f));
        Vector3f const coord011(coord001 + Vector3f(0.0f, blocksize, 0.0f));
        Vector3f const coord111(coord001 + Vector3f(blocksize, blocksize, 0.0f));

        Vector3f const normal0 = (coord100 - coord000).crossProduct(coord010 - coord000);
        Vector3f const normal1 = (coord011 - coord000).crossProduct(coord001 - coord000);
        Vector3f const normal2 = (coord111 - coord010).crossProduct(coord011 - coord010);

        // front face
        unsigned int offset = vbodata.size();
        vbodata.emplace_back(coord000, normal0);
        vbodata.emplace_back(coord010, normal0);
        vbodata.emplace_back(coord110, normal0);
        vbodata.emplace_back(coord100, normal0);
        ibodata.emplace_back(offset + 0);
        ibodata.emplace_back(offset + 1);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 3);
        ibodata.emplace_back(offset + 0);

        // back face
        offset = vbodata.size();
        vbodata.emplace_back(coord001, -normal0);
        vbodata.emplace_back(coord011, -normal0);
        vbodata.emplace_back(coord111, -normal0);
        vbodata.emplace_back(coord101, -normal0);
        ibodata.emplace_back(offset + 0);
        ibodata.emplace_back(offset + 3);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 1);
        ibodata.emplace_back(offset + 0);

        // Side face 1
        offset = vbodata.size();
        vbodata.emplace_back(coord000, normal1);
        vbodata.emplace_back(coord001, normal1);
        vbodata.emplace_back(coord011, normal1);
        vbodata.emplace_back(coord010, normal1);
        ibodata.emplace_back(offset + 0);
        ibodata.emplace_back(offset + 1);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 3);
        ibodata.emplace_back(offset + 0);

        // Side face 2
        offset = vbodata.size();
        vbodata.emplace_back(coord100, -normal1);
        vbodata.emplace_back(coord101, -normal1);
        vbodata.emplace_back(coord111, -normal1);
        vbodata.emplace_back(coord110, -normal1);
        ibodata.emplace_back(offset + 0);
        ibodata.emplace_back(offset + 3);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 1);
        ibodata.emplace_back(offset + 0);

        // top face
        offset = vbodata.size();
        vbodata.emplace_back(coord010, normal2);
        vbodata.emplace_back(coord011, normal2);
        vbodata.emplace_back(coord111, normal2);
        vbodata.emplace_back(coord110, normal2);
        ibodata.emplace_back(offset + 0);
        ibodata.emplace_back(offset + 1);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 3);
        ibodata.emplace_back(offset + 0);

        // bottom face
        offset = vbodata.size();
        vbodata.emplace_back(coord000, -normal2);
        vbodata.emplace_back(coord001, -normal2);
        vbodata.emplace_back(coord101, -normal2);
        vbodata.emplace_back(coord100, -normal2);
        ibodata.emplace_back(offset + 0);
        ibodata.emplace_back(offset + 3);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 2);
        ibodata.emplace_back(offset + 1);
        ibodata.emplace_back(offset + 0);
      }
    }
  }

  vbodata.shrink_to_fit();
  ibodata.shrink_to_fit();
  buf.setup(vbodata, ibodata);
}
