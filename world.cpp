//#include "world.h"
#include <iostream>
#include "chunk.h"
#include "entity/playership.h"
#include "gameplayer.h"
#include "universe.h"

extern gameplayer player;
extern universe root;

world::world() {
  /// Default constructor
  // ugly but efficient way to initialise a 3D vector
  chunks = std::vector<std::vector<std::vector<chunk*>>>(size, std::vector<std::vector<chunk*>>(size, std::vector<chunk*>(size, nullptr)));
}

world::~world() {
  /// Default destructor
  clear_chunks();
}

void world::correct_chunk_coords(vector3i &chunk_coords) {
  /// Wrap the chunk coords round if they exceed the bounds
  chunk_coords.x = ((size + chunk_coords.x) % size);                            // we can use modulo to do this branchlessly
  chunk_coords.y = ((size + chunk_coords.y) % size);
  chunk_coords.z = ((size + chunk_coords.z) % size);
}

chunk *world::get_chunk(vector3i const &chunk_coords) {
  /// Find a chunk with the given coordinates, and generate it if it doesn't exist
  vector3i checked_coords(chunk_coords);
  correct_chunk_coords(checked_coords);
  chunk *&thischunk = chunks[checked_coords.x][checked_coords.y][checked_coords.z];
  if(!thischunk) {
    thischunk = new chunk(checked_coords, *this);
    //std::cout << "DEBUG: Created chunk at " << checked_coords << std::endl;
  }
  return thischunk;
}

void world::find_visible_chunks(vector3i const &chunk_coords,
                                quatf const &view_direction,
                                int range) {
  /// Return a list of chunks visible in this direction from a given chunk, in optimal rendering order
  visible_chunks.clear();
  visible_chunks.reserve(std::pow((range * 2) + 1, 3));

  vector3f view_vector(0.0, 0.0, -1.0);
  view_vector.rotate(view_direction);
  float constexpr view_cull_threshold1 = -0.5;                                  // columns - tweak this depending on FOV angle
  float constexpr view_cull_threshold2 = -0.5;                                  // planes  - tweak this depending on FOV angle
  float constexpr view_cull_threshold3 = -0.5;                                  // corners - tweak this depending on FOV angle

  // the central chunk always comes first - the order here makes for optimal occlusion testing later
  visible_chunks.emplace_back(get_chunk(chunk_coords));

  // then the cardinal columns, working outwards - they will always overlap the corners
  for(int x =  1; x <=  range; ++x) {
    if(view_vector.x >  view_cull_threshold1) {                                 // primitive view culling scheme
      visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, 0, 0)));
    }
  }
  for(int x = -1; x >= -range; --x) {
    if(view_vector.x < -view_cull_threshold1) {
      visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, 0, 0)));
    }
  }
  for(int y =  1; y <=  range; ++y) {
    if(view_vector.y >  view_cull_threshold1) {
      visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, y, 0)));
    }
  }
  for(int y = -1; y >= -range; --y) {
    if(view_vector.y < -view_cull_threshold1) {
      visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, y, 0)));
    }
  }
  for(int z =  1; z <=  range; ++z) {
    if(view_vector.z >  view_cull_threshold1) {
      visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, 0, z)));
    }
  }
  for(int z = -1; z >= -range; --z) {
    if(view_vector.z < -view_cull_threshold1) {
      visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, 0, z)));
    }
  }

  // then add the rest of the cardinal planes
  for(int y =  1; y <=  range; ++y) {                                           // x plane
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.y >  view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {                               // primitive view culling scheme
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, y, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.y >  view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, y, z)));
      }
    }
  }
  for(int y = -1; y >= -range; --y) {
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.y < -view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, y, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.y < -view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(0, y, z)));
      }
    }
  }
  for(int x =  1; x <=  range; ++x) {                                           // z plane
    for(int y =  1; y <=  range; ++y) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.y >  view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, 0)));
      }
    }
    for(int y = -1; y >= -range; --y) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.y < -view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, 0)));
      }
    }
  }
  for(int x = -1; x >= -range; --x) {
    for(int y =  1; y <=  range; ++y) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.y >  view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, 0)));
      }
    }
    for(int y = -1; y >= -range; --y) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.y < -view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, 0)));
      }
    }
  }
  for(int x =  1; x <=  range; ++x) {                                           // y plane
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, 0, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.x >  view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, 0, z)));
      }
    }
  }
  for(int x = -1; x >= -range; --x) {
    for(int z =  1; z <=  range; ++z) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.z >  view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, 0, z)));
      }
    }
    for(int z = -1; z >= -range; --z) {
      if(view_vector.x < -view_cull_threshold2 &&
         view_vector.z < -view_cull_threshold2) {
        visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, 0, z)));
      }
    }
  }

  // finally add the corners, working outwards
  for(int x =  1; x <=  range; ++x) {
    for(int y =  1; y <=  range; ++y) {
      for(int z =  1; z <=  range; ++z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y >  view_cull_threshold3 &&
           view_vector.z >  view_cull_threshold3) {                             // primitive view culling scheme
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y >  view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
        }
      }
    }
    for(int y = -1; y >= -range; --y) {
      for(int z =  1; z <=  range; ++z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z >  view_cull_threshold3) {
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x >  view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
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
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x < -view_cull_threshold3 &&
           view_vector.y >  view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
        }
      }
    }
    for(int y = -1; y >= -range; --y) {
      for(int z =  1; z <=  range; ++z) {
        if(view_vector.x < -view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z >  view_cull_threshold3) {
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
        }
      }
      for(int z = -1; z >= -range; --z) {
        if(view_vector.x < -view_cull_threshold3 &&
           view_vector.y < -view_cull_threshold3 &&
           view_vector.z < -view_cull_threshold3) {
          visible_chunks.emplace_back(get_chunk(chunk_coords + vector3i(x, y, z)));
        }
      }
    }
  }

  //std::cout << "DEBUG: visible_chunks.size() " << visible_chunks.size() << " / " << std::pow((range * 2) + 1, 3) << " (" << visible_chunks.size() * 100 / std::pow((range * 2) + 1, 3) << "% drawn)" << std::endl;
}

vector3f world::check_collision(vector3i const &chunk_coords,
                                vector3f const &coords,
                                float radius) {
  /// Check if a given point is colliding, and if so, return a normal vector to the collision surface
  vector3f normal = get_chunk(chunk_coords)->check_collision(coords, radius);
  if(__builtin_expect(normal != vector3f(0.0, 0.0, 0.0), 0)) {                  // branch prediction hint: unlikely (the usual case will be no collision)
    return normal;
  }
  // TODO: if we haven't collided, check any other chunks our radius overlaps onto.
  // Currently disabled because this breaks things.
  /*
  if(coords.x + radius > chunk::size) {
    normal = get_chunk(chunk_coords + vector3i(1, 0, 0))->check_collision(coords - chunk::size, radius);
  } else if(coords.x - radius < 0.0) {
    normal = get_chunk(chunk_coords + vector3i(-1, 0, 0))->check_collision(coords + chunk::size, radius);
  }
  if(coords.y + radius > chunk::size) {
    normal = get_chunk(chunk_coords + vector3i(0, 1, 0))->check_collision(coords - chunk::size, radius);
  } else if(coords.y - radius < 0.0) {
    normal = get_chunk(chunk_coords + vector3i(0, -1, 0))->check_collision(coords + chunk::size, radius);
  }
  if(coords.z + radius > chunk::size) {
    normal = get_chunk(chunk_coords + vector3i(0, 0, 1))->check_collision(coords - chunk::size, radius);
  } else if(coords.z - radius < 0.0) {
    normal = get_chunk(chunk_coords + vector3i(0, 0, -1))->check_collision(coords + chunk::size, radius);
  }
  */
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

void world::preload_chunks() {
  /// Chunk pre-loading
  float constexpr chunks_to_load = world::size * world::size * world::size;
  float chunks_done = 0;
  for(vector3i c;  c.x != size; ++c.x) {
    for(  c.y = 0; c.y != size; ++c.y) {
      for(c.z = 0; c.z != size; ++c.z) {
        float const progress(++chunks_done / chunks_to_load);
        std::stringstream ss;
        ss << static_cast<unsigned int>(progress * 100.0f) << "ing...";
        root.render_progressscreen(progress, ss.str());
        get_chunk(c);
      }
    }
  }
}

void world::clear_chunks() {
  /// Destroy all chunks
  for(auto &x : chunks) {
    for(auto &y : x) {
      for(auto &z : y) {
        delete z;
      }
    }
  }
  chunks.clear();
}

void world::clear_entities() {
  /// Clears all entities including the player ship.
  for(auto ent: entities) {
    //std::cout << "DEBUG: Removing entity: " << ent << std::endl;
    ent->get_parent()->remove_entity(ent);
  }
  entities.clear();
}

void world::update() {
  /// Update visible chunks and entities.
  for(auto thischunk: visible_chunks) {
    thischunk->update();
  }
  for(unsigned int i = 0; i < entities.size(); ++i) {                           // Not using a foreach/iterator here because entities can be created during this loop if updating an entity causes a new chunk to be created.
    entity *ent = entities[i];
    ent->update();
    if(__builtin_expect(ent->energy <= 0 && ent != player.current_ship, 0)) {
      ent->destroy();
      if(ent->get_entity_type() == entity::entity_type::CORE) {
        player.current_ship->cores_destroyed++;
        std::cout << "DEBUG: Core destroyed!" << std::endl;
      }
    }
  }
  //std::cout << "DEBUG: Player coords: " << player.current_ship->get_world_position() << std::endl;

  // Destroy any dead (non-player) entities
  for(auto it = entities.begin(); it != entities.end();) {
    entity *ent = *it;
    if(__builtin_expect(ent->energy <= 0 && ent != player.current_ship, 0)) {
      //std::cout << "DEBUG: Removing entity: " << ent << std::endl;
      it = entities.erase(it);
      ent->get_parent()->remove_entity(ent);
      delete ent;
    } else {
      ++it;
    }
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

void world::render(vector3i const &chunk_coords,
                   quatf const &view_direction) {
  /// Draw the relevant portions of this world
  find_visible_chunks(chunk_coords, view_direction, 4);                         // refresh the list of visible chunks
  for(auto const &c : visible_chunks) {
    c->render(chunk_coords);
  }
}
