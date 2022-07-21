#ifndef MATERIAL_MESSENGER_HPP
#define MATERIAL_MESSENGER_HPP
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

namespace ne697 {
  // Forward declaration, to resolve circular dependency with DetectorConstruction
  // You still need to #include "detectorconstruction.hpp" in
  // geometrymessenger.cpp
  class DetectorConstruction;

  // User-facing part of the UI: just for the user
  class MaterialMessenger: public G4UImessenger {
  public:
    MaterialMessenger(DetectorConstruction* dc);
    ~MaterialMessenger();

    void SetNewValue(G4UIcommand* cmd, G4String val) override final;

  private:
    DetectorConstruction* m_dc;
    G4UIdirectory* m_directory;
    G4UIcmdWithAString* m_detMaterialCmd;
    G4UIcmdWithAString* m_worldMaterialCmd;

  };
}

#endif
