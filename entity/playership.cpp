#include "playership.h"
#include "weapon.h"

playership::playership(world &parent_world,
             chunk *parent_chunk,
             Vector3f const &position,
             Quatf const &orientation)
  : ship(parent_world, parent_chunk, position, orientation) {
  /// Default constructor
  energy = max_energy;
}

playership::~playership() {
  /// Default destructor
}

void playership::update() {
  if (energy < max_energy) {
    energy++;
  }
  #ifndef NDEBUG
  if (invincible) {
    energy = max_energy;
  }
  #endif
  ship::update();
}

void playership::accelerate(Vector3f accel) {
  /// Accelerate this ship along a given vector relative to its orientation
  if (energy > accel.length() + 1) {
    energy -= static_cast<int>(accel.length());
    ship::accelerate(accel);
  }
}

bool playership::fire(unsigned int weapon_id) {
  /// Fire the selected weapon

  #ifndef NDEBUG
    if(weapon_id >= weapons.size()) {
      std::cout << "ERROR: " << __PRETTY_FUNCTION__ << " tried to access index " << weapon_id << " of " << weapons.size() << std::endl;
      return false;
    }
  #endif

  if (energy > weapons[weapon_id]->get_cost_per_shot() && ship::fire(weapon_id)) {
    energy -= weapons[weapon_id]->get_cost_per_shot();
    return true;
  }
  return false;
}

void playership::collided_with(entity *other) {
  #ifndef NDEBUG
    if (!invincible) {
      entity::collided_with(other);
    }
  #else
    entity::collided_with(other);
  #endif
}

entity::entity_type playership::get_entity_type() const {
  return entity::entity_type::PLAYER;
}

