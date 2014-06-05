#include "weapon.h"
#include <iostream>

weapon::weapon(ship *parent)
  : parent(parent) {
  /// Default constructor
}

weapon::~weapon() {
  /// Default destructor
}

void weapon::update() {
  /// Tick this weapon's cooldown
  if(cooldown != 0) {
    --cooldown;
  }
}

bool weapon::fire() {
  /// Fire this weapon
  std::cout << "ERROR: virtual base function " << __PRETTY_FUNCTION__ << " called; this should never happen" << std::endl;
}

unsigned int weapon::get_cost_per_shot() const {
  return 50;
}
