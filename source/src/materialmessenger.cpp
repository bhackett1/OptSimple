#include "materialmessenger.hpp"
#include "detectorconstruction.hpp"
#include "G4UnitsTable.hh"

namespace ne697 {
  MaterialMessenger::MaterialMessenger(DetectorConstruction* dc):
    m_dc(dc)
  {
    // Directory: /ne697/material
    m_directory = new G4UIdirectory("/ne697/material/");
    m_directory->SetGuidance("Change material of detector.");

    // Set detector material: /ne697/material/det_material
    m_detMaterialCmd = new G4UIcmdWithAString("/ne697/material/det_material", this);
    m_detMaterialCmd->SetGuidance("Set the detector material.");
    m_detMaterialCmd->SetParameterName("material", true);
    m_detMaterialCmd->SetDefaultValue(m_dc->get_det_material());
    m_detMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    // Set world material: /ne697/material/world_material
    m_worldMaterialCmd = new G4UIcmdWithAString("/ne697/material/world_material", this);
    m_worldMaterialCmd->SetGuidance("Set the world material.");
    m_worldMaterialCmd->SetParameterName("material", true);
    m_worldMaterialCmd->SetDefaultValue(m_dc->get_world_material());
    m_worldMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
  }

  MaterialMessenger::~MaterialMessenger() {
    delete m_directory;
    delete m_detMaterialCmd;
    delete m_worldMaterialCmd;
  }

  void MaterialMessenger::SetNewValue(G4UIcommand* cmd, G4String val) {
    if (cmd == m_detMaterialCmd) {
      m_dc->set_det_material(val);
      G4cout << "Detector material set to " << val
        << G4endl;
    }
    if (cmd == m_worldMaterialCmd) {
      m_dc->set_world_material(val);
      G4cout << "World material set to " << val
        << G4endl;
    }

    // Command didn't match
    return;
  }
}
