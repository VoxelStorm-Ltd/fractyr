#include "grunt.h"
#include "buffers/buffer_enemy_grunt.h"
#include "weapon/weapon.h"
#include "gameplayer.h"
#include "entity/playership.h"
#include "world.h"
#include "chunk.h"
#include "entity/particle/particle.h"

extern gameplayer player;

buffer_enemy_grunt grunt::buf;

grunt::grunt(world &parent_world,
             chunk *parent_chunk,
             vector3f const &position,
             quatf const &orientation,
             float scale)
  : enemy(parent_world, parent_chunk, position, orientation),
    scale(scale) {
  /// Default constructor
  radius = 3.0 * scale;
  mass = 1000.0 * scale;
  energy = 400 * scale * scale;
}

grunt::~grunt() {
  /// Default destructor
}

void grunt::render() const {
  /// Render this entity
  glPushMatrix();
  glTranslatef(position.x, position.y, position.z);
  glMultMatrixf(orientation_conjugate.transform());
  glScalef(scale, scale, scale);
  buf.render();
  glPopMatrix();
}

void grunt::update() {
  /// Update this enemy's AI actions
  // TODO: The tracking gets confused if (I think) the player crosses the z axis, slerp should deal with this fine but isn't for some reason.

  vector3f aimpos = -get_offset(*player.current_ship);

  #ifndef NDEBUG
    if(player.invisible) {
      orientation *= quatf::from_axis_rot(vector3f(0.0, 1.0, 0.0), 0.1);          // spin gently
      orientation_conjugate = orientation.conjugate_copy();
      enemy::update();
      return;
    }
  #endif

  if(__builtin_expect(aimpos.length_sq() < 10000, 0)) {
    aimpos += (player.current_ship->get_velocity() - velocity) * (aimpos.length() / (weapons[0]->get_shot_speed() * 2.0));
    velocity.x += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
    velocity.y += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
    velocity.z += (rand()  /static_cast<float>(RAND_MAX) - 0.5) / 100.0;

    quatf const &target_orientation = quatf::from_matrix(matrix4f::create_look_at(vector3f(0.0, 0.0, 0.0), aimpos, vector3f(0.0, 0.0, 1.0)));
    orientation = orientation.slerp(0.05, target_orientation);
    orientation.normalise();
    orientation_conjugate = orientation.conjugate_copy();

    for(auto weapon : weapons) {
      weapon->fire();
    }
  } else {
    velocity = vector3f(0.0, 0.0, 0.0);
  }
  enemy::update();
}

void grunt::destroy() {
  /// Make an explosion
  for(unsigned int p = 0; p != 40; ++p) {
    new particle(parent_world,
                 parent,
                 position,
                 orientation,
                 velocity + vector3f((static_cast<float>(rand()) / RAND_MAX) - 0.5,
                                     (static_cast<float>(rand()) / RAND_MAX) - 0.5,
                                     (static_cast<float>(rand()) / RAND_MAX) - 0.5) * 0.5);
  }
}

float grunt::get_collision_damage() const {
  return 100.0;
}
