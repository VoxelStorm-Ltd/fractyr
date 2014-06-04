#ifndef BLASTER_H_INCLUDED
#define BLASTER_H_INCLUDED

#include "weapon.h"
#include "buffer_plasma.h"

class blaster : public weapon {
  /// Basic classic video game slow-moving bullet blaster
public:
  blaster(ship *parent);
  ~blaster();

  void fire() override final;
};

#endif // BLASTER_H_INCLUDED
