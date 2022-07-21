#include "pga.hpp"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "gunmessenger.hpp"


namespace ne697 {
  PGA::PGA():
    G4VUserPrimaryGeneratorAction(),
    m_gun(new G4ParticleGun(1)),
    m_offset(30*cm)
  {
    G4cout << "Creating PGA" << G4endl;
    m_messenger = new GunMessenger(this);
    m_gun->SetParticleDefinition(G4Gamma::Definition());
    m_gun->SetParticleEnergy(1.*MeV);
    //Set direction to be -Y axis
    // m_gun->SetParticleMomentumDirection(G4ThreeVector(0., -1., 0.));

  }

  PGA::~PGA() {
    G4cout << "Deleting PGA" << G4endl;
    delete m_messenger;
    delete m_gun;
  }

  void PGA::GeneratePrimaries(G4Event* event) {
    //
    // G4double x_pos = (G4UniformRand()-0.5)*50.0;
    // G4double z_pos = (G4UniformRand()-0.5)*50.0;

    //Print statement for debugging
    // G4cout<< "These are the coordinates: "<<x_pos<<"  "
    // 	    				  <<30.0 <<"  "
    // 	    				  <<z_pos<<G4endl;
    //Generate random position of particle within region of interest
    // m_gun->SetParticlePosition(G4ThreeVector(x_pos*cm, m_offset, z_pos*cm));
    m_gun->SetParticlePosition(G4ThreeVector(0.*cm, 0.*cm, 0.*cm));
    m_gun->GeneratePrimaryVertex(event);
    return;
  }

  void PGA::set_gun_offset(G4double const& offset) {
    m_offset = offset;
    return;
  }

  G4double const& PGA::get_gun_offset() const {
    return m_offset;
  }
}
