#ifndef WEAPON_H_INCLUDED
#define WEAPON_H_INCLUDED

class ship;

class weapon {
  /// Polymorphic base class for ship-mounted weapons
protected:
  unsigned int cooldown = 0;                // how many frames left until it can fire again

  ship *parent = nullptr;                   // what ship it belongs to, if any

protected:
  weapon(ship *parent);
public:
  virtual ~weapon();

  virtual void update();
  virtual void fire();
};

#endif // WEAPON_H_INCLUDED
