#ifndef PGA_HPP
#define PGA_HPP

#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "detectorconstruction.hpp"

namespace ne697 {

  class GunMessenger;

  class PGA: public G4VUserPrimaryGeneratorAction {
    public:
      PGA();
      ~PGA();

      void GeneratePrimaries(G4Event* event) override final;

      void set_gun_offset(G4double const& offset);
      G4double const& get_gun_offset() const;

    private:
      G4ParticleGun* m_gun;

      DetectorConstruction* m_geo;

      GunMessenger* m_messenger;

      G4double m_offset;
  };
}

#endif
