#include "world.h"
#include <algorithm>
#include "vmath.h"
#include "chunk.h"
#include "entity.h"

world::world() {
  /// Default constructor
  // ugly but efficient way to initialise a 3D vector
  chunks = std::vector<std::vector<std::vector<chunk*>>>(size, std::vector<std::vector<chunk*>>(size, std::vector<chunk*>(size, nullptr)));
}

world::~world() {
  /// Default destructor
  for(auto &x : chunks) {
    for(auto &y : x) {
      for(auto &z : y) {
        delete z;
      }
    }
  }
}

chunk *world::get_chunk(Vector3i const &chunk_coords) {
  /// Find a chunk with the given coordinates, and generate it if it doesn't exist
  Vector3i const checked_coords((size + chunk_coords.x) % size,
                                (size + chunk_coords.y) % size,
                                (size + chunk_coords.z) % size);     // the world wraps, so we just modulo
  chunk *&thischunk = chunks[checked_coords.x][checked_coords.y][checked_coords.z];
  if(!thischunk) {
    thischunk = new chunk(checked_coords, *this);
    //std::cout << "DEBUG: Created chunk at " << checked_coords << std::endl;
  }
  return thischunk;
}

std::vector<chunk*> world::get_visible_chunks(Vector3i const &chunk_coords,
                                              Quatf const &view_direction,
                                              int range) {
  /// Return a list of chunks visible in this direction from a given chunk, in optimal rendering order
  std::vector<chunk*> chunk_list;
  chunk_list.reserve(pow((range * 2) + 1, 3));

  Vector3f view_vector(0.0, 0.0, -1.0);
  view_vector.rotate(view_direction);
  float constexpr view_cull_threshold1 = -0.5;    // columns - tweak this depending on FOV angle
  float constexpr view_cull_threshold2 = -0.5;    // planes  - tweak this depending on FOV angle
  float constexpr view_cull_threshold3 = -0.5;    // corners - tweak this depending on FOV angle

  // the central chunk always comes first - the order here makes for optimal occlusion testing later
  chunk_list.emplace_back(get_chunk(chunk_coords));

  // then the cardinal columns, working outwards - they will always overlap the corners
  for(int x =  1; x <=  range; ++x) {
    if(view_vector.x >  view_cull_threshold1) {       // primitive view culling scheme
      chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, 0, 0)));
    }
  }
  for(int x = -1; x >= -range; --x) {
    if(view_vector.x < -view_cull_threshold1) {
      chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, 0, 0)));
    }
  }
  for(int y =  1; y <=  range; ++y) {
    if(view_vector.y >  view_cull_threshold1) {
      chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, y, 0)));
    }
  }
  for(int y = -1; y >= -range; --y) {
    if(view_vector.y < -view_cull_threshold1) {
      chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, y, 0)));
    }
  }
  for(int z =  1; z <=  range; ++z) {
    if(view_vector.z >  view_cull_threshold1) {
      chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, 0, z)));
    }
  }
  for(int z = -1; z >= -range; --z) {
    if(view_vector.z < -view_cull_threshold1) {
      chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, 0, z)));
    }
  }

  // then add the rest of the cardinal planes
  for(int y =  1; y <=  range; ++y) {     // x plane
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.y >  view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {       // primitive view culling scheme
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, y, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.y >  view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, y, z)));
      }
    }
  }
  for(int y = -1; y >= -range; --y) {
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.y < -view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, y, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.y < -view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(0, y, z)));
      }
    }
  }
  for(int x =  1; x <=  range; ++x) {     // z plane
    for(int y =  1; y <=  range; ++y) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.y >  view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, 0)));
      }
    }
    for(int y = -1; y >= -range; --y) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.y < -view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, 0)));
      }
    }
  }
  for(int x = -1; x >= -range; --x) {
    for(int y =  1; y <=  range; ++y) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.y >  view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, 0)));
      }
    }
    for(int y = -1; y >= -range; --y) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.y < -view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, 0)));
      }
    }
  }
  for(int x =  1; x <=  range; ++x) {     // y plane
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, 0, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, 0, z)));
      }
    }
  }
  for(int x = -1; x >= -range; --x) {
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, 0, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, 0, z)));
      }
    }
  }

  // finally add the corners, working outwards
  for(int x =  1; x <=  range; ++x) {
    for(int y =  1; y <=  range; ++y) {
      for(int z =  1; z <=  range; ++z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y >  view_cull_threshold3 &&
           view_vector.z >  view_cull_threshold3) {       // primitive view culling scheme
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y >  view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
    }
    for(int y = -1; y >= -range; --y) {
      for(int z =  1; z <=  range; ++z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z >  view_cull_threshold3) {
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
    }
  }
  for(int x = -1; x >= -range; --x) {
    for(int y =  1; y <=  range; ++y) {
      for(int z =  1; z <=  range; ++z) {
        if(view_vector.x < -view_cull_threshold3 &&
           view_vector.y >  view_cull_threshold3 &&
           view_vector.z >  view_cull_threshold3) {
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x < -view_cull_threshold3 &&
           view_vector.y >  view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
    }
    for(int y = -1; y >= -range; --y) {
      for(int z =  1; z <=  range; ++z) {
        if(view_vector.x < -view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z >  view_cull_threshold3) {
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x < -view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          chunk_list.emplace_back(get_chunk(chunk_coords + Vector3i(x, y, z)));
        }
      }
    }
  }

  //std::cout << "DEBUG: chunk_list.size() " << chunk_list.size() << " / " << pow((range * 2) + 1, 3) << " (" << chunk_list.size() * 100 / pow((range * 2) + 1, 3) << "% drawn)" << std::endl;
  return chunk_list;
}

Vector3f world::check_collision(Vector3i const &chunk_coords,
                                Vector3f const &coords,
                                float radius) {
  /// Check if a given point is colliding, and if so, return a normal vector to the collision surface
  Vector3f normal = get_chunk(chunk_coords)->check_collision(coords, radius);
  if(__builtin_expect(normal != Vector3f(0.0, 0.0, 0.0), 0)) {      // branch prediction hint: unlikely (the usual case will be no collision)
    return normal;
  }
  // if we haven't collided, check any other chunks our radius overlaps onto
  if(coords.x + radius > chunk::size) {
    normal = get_chunk(chunk_coords + Vector3i(1, 0, 0))->check_collision(coords - chunk::size, radius);
  } else if(coords.x - radius < 0.0) {
    normal = get_chunk(chunk_coords + Vector3i(-1, 0, 0))->check_collision(coords + chunk::size, radius);
  }
  if(coords.y + radius > chunk::size) {
    normal = get_chunk(chunk_coords + Vector3i(0, 1, 0))->check_collision(coords - chunk::size, radius);
  } else if(coords.y - radius < 0.0) {
    normal = get_chunk(chunk_coords + Vector3i(0, -1, 0))->check_collision(coords + chunk::size, radius);
  }
  if(coords.z + radius > chunk::size) {
    normal = get_chunk(chunk_coords + Vector3i(0, 0, 1))->check_collision(coords - chunk::size, radius);
  } else if(coords.z - radius < 0.0) {
    normal = get_chunk(chunk_coords + Vector3i(0, 0, -1))->check_collision(coords + chunk::size, radius);
  }
  return normal;
}

void world::delete_buffers() {
  for(auto &x : chunks) {
    for(auto &y : x) {
      for(auto &z : y) {
        if(z) {
          z->delete_buffers();
        }
      }
    }
  }
}
void world::setup_buffers() {
  for(auto &x : chunks) {
    for(auto &y : x) {
      for(auto &z : y) {
        if(z) {
          z->setup_buffers();
        }
      }
    }
  }
}

void world::update() {
  /// Update every chunk in this world
  /*
  for(unsigned int x = 0; x != size; ++x) {
    for(unsigned int y = 0; y != size; ++y) {
      for(unsigned int z = 0; z != size; ++z) {
        chunk *thischunk = chunks[x][y][z];
        if(thischunk) {
          thischunk->update();
        }
      }
    }
  }
  */
  // the above is waaaaay too slow!
  for(auto &e : entities) {
    e->update();
  }
}

void world::add_entity(entity *thisentity) {
  /// Add this entity to our list
  entities.push_back(thisentity);
}

void world::remove_entity(entity *thisentity) {
  /// Take this entity out of our list
  //entities.erase(std::remove(entities.begin(), entities.end(), thisentity), entities.end());
  // should be faster:
  entities.erase(std::find(entities.begin(), entities.end(), thisentity));
  // TODO: for larger entity lists, instead sort and use std::binary_search
}

void world::render(Vector3i const &chunk_coords,
                   Quatf const &view_direction) {
  /// Draw the relevant portions of this world
  std::vector<chunk*> const &chunks_to_render = get_visible_chunks(chunk_coords, view_direction, 2);    // view range
  for(auto const &c : chunks_to_render) {
    c->render(chunk_coords);
  }
}
