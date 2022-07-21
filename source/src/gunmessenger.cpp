#include "gunmessenger.hpp"
#include "pga.hpp"
#include "G4UnitsTable.hh"

namespace ne697 {
  GunMessenger::GunMessenger(PGA* pga):
    m_pga(pga)
  {
    // Directory: /ne697/geometry
    m_directory = new G4UIdirectory("/ne697/gun/");
    m_directory->SetGuidance("Change parameters of the particle gun.");

    // Set detector thickness: /ne697/geometry/det_thickness
    m_gunOffsetCmd = new G4UIcmdWithADoubleAndUnit("/ne697/gun/offset", this);
    m_gunOffsetCmd->SetGuidance("Set the particle gun height.");
    m_gunOffsetCmd->SetParameterName("y", true);
    m_gunOffsetCmd->SetDefaultValue(m_pga->get_gun_offset());
    m_gunOffsetCmd->AvailableForStates(G4State_PreInit);
  }

  GunMessenger::~GunMessenger() {
    delete m_directory;
    delete m_gunOffsetCmd;
  }

  void GunMessenger::SetNewValue(G4UIcommand* cmd, G4String val) {
    if (cmd == m_gunOffsetCmd) {
      G4double parsed_val = m_gunOffsetCmd->GetNewDoubleValue(val);
      // This is fine too - just being clear above
      //auto parsed_val = m_detSizeCmd->GetNew3VectorValue(val);
      if(parsed_val< 0.0)
        { G4cerr <<"Error: Gun height must be greater than zero!"<< G4endl; }

      m_pga->set_gun_offset(parsed_val);
      G4cout << "Particle gun height set to: " << G4BestUnit(parsed_val, "Length")
        << G4endl;
    }

    // Command didn't match
    return;
  }
}
