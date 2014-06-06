#include "grunt.h"
#include "buffer_enemy_grunt.h"
#include "weapon.h"
#include "gameplayer.h"
#include "playership.h"
#include "world.h"
#include "chunk.h"
#include "particle.h"

extern gameplayer player;

buffer_enemy_grunt grunt::buf;

grunt::grunt(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation,
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

  Vector3f aimpos = player.current_ship->get_world_position() - get_world_position();
  aimpos += (player.current_ship->get_velocity() - velocity) * (aimpos.length() / (weapons[0]->get_shot_speed() * 2.0));

  float constexpr worldsize = world::size * chunk::size;
  float constexpr halfworld = worldsize / 2.0;

  // Wrap aim position around the world.
  if(aimpos.x >  halfworld) aimpos.x -= worldsize;
  if(aimpos.y >  halfworld) aimpos.y -= worldsize;
  if(aimpos.z >  halfworld) aimpos.z -= worldsize;
  if(aimpos.x < -halfworld) aimpos.x += worldsize;
  if(aimpos.y < -halfworld) aimpos.y += worldsize;
  if(aimpos.z < -halfworld) aimpos.z += worldsize;

  #ifndef NDEBUG
    if(player.invisible) {
      orientation *= Quatf::fromAxisRot(Vector3f(0.0, 1.0, 0.0), 0.1);    // spin gently
      orientation_conjugate = orientation.conjugate_copy();
      enemy::update();
      return;
    }
  #endif

  if(aimpos.lengthSq() < 10000) {
    velocity.x += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
    velocity.y += (rand() / static_cast<float>(RAND_MAX) - 0.5) / 100.0;
    velocity.z += (rand()  /static_cast<float>(RAND_MAX) - 0.5) / 100.0;

    Quatf const &target_orientation = Quatf::fromMatrix(Matrix4f::createLookAt(Vector3f(0.0, 0.0, 0.0), aimpos, Vector3f(0.0, 0.0, 1.0)));
    orientation = orientation.slerp(0.05, target_orientation);
    orientation.normalise();
    orientation_conjugate = orientation.conjugate_copy();

    for (auto weapon : weapons) {
      weapon->fire();
    }
  } else {
    velocity = Vector3f(0.0, 0.0, 0.0);
  }
  enemy::update();
}

void grunt::destroy() {
  /// Make an explosion
  for(unsigned int p = 0; p != 20; ++p) {
    new particle(parent_world,
                 parent,
                 position,
                 orientation,
                 Vector3f((rand() % 4) - 8,
                          (rand() % 4) - 8,
                          (rand() % 4) - 8));
  }
}

float grunt::get_collision_damage() const {
  return 100.0;
}
