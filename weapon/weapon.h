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
  virtual bool fire();                      // attempts to fire weapon, returns true if the weapon fired, false otherwise.
  virtual unsigned int get_cost_per_shot() const;
};

#endif // WEAPON_H_INCLUDED
