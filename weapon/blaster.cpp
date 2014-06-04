#include "blaster.h"
#include "plasma.h"
#include "ship.h"

blaster::blaster(ship *parent)
  : weapon(parent) {
  /// Default constructor
}

blaster::~blaster() {
  /// Default destructor
}

void blaster::fire() {
  /// Fire this weapon
  if(cooldown != 0) {
    return;
  }
  // position this at our ship
  Vector3f barrel_coords(parent->get_weapon_position());
  chunk *thischunk(parent->get_parent());
  parent->correct_point(barrel_coords, thischunk);

  plasma *thisbullet = new plasma(*parent->get_parent_world(), thischunk, barrel_coords, parent->get_orientation(), parent->get_velocity());
  cooldown = 60 / 2;
  std::cout << "DEBUG: fired blaster" << std::endl;
}
