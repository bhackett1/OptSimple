#include "geometrymessenger.hpp"
#include "detectorconstruction.hpp"
#include "G4UnitsTable.hh"

namespace ne697 {
  GeometryMessenger::GeometryMessenger(DetectorConstruction* dc):
    m_dc(dc)
  {
    // Directory: /ne697/geometry
    m_directory = new G4UIdirectory("/ne697/geometry/");
    m_directory->SetGuidance("Change parameters of the geometry.");

    // Set detector thickness: /ne697/geometry/det_thickness
    m_detThicknessCmd = new G4UIcmdWithADoubleAndUnit("/ne697/geometry/det_thickness", this);
    m_detThicknessCmd->SetGuidance("Set the detector thickness.");
    m_detThicknessCmd->SetParameterName("y", true);
    m_detThicknessCmd->SetDefaultValue(m_dc->get_det_thickness());
    m_detThicknessCmd->AvailableForStates(G4State_PreInit);

    // Set detector length: /ne697/geometry/det_length
    m_detRadiusCmd = new G4UIcmdWithADoubleAndUnit("/ne697/geometry/det_radius", this);
    m_detRadiusCmd->SetGuidance("Set the photon detector radius.");
    m_detRadiusCmd->SetParameterName("r", true);
    m_detRadiusCmd->SetDefaultValue(m_dc->get_det_radius());
    m_detRadiusCmd->AvailableForStates(G4State_PreInit);

    m_detGeometryCmd = new G4UIcmdWithAString("/ne697/geometry/det_geometry", this);
    m_detGeometryCmd->SetGuidance("Select geometry of the photon detector.");
    m_detGeometryCmd->SetGuidance("Only accepts 'Sphere' or 'Cylinder'");
    m_detGeometryCmd->SetCandidates("Sphere Cylinder");
    m_detGeometryCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    m_detGeometryCmd->SetDefaultValue(m_dc->get_det_geometry());

  }

  GeometryMessenger::~GeometryMessenger() {
    delete m_directory;
    delete m_detThicknessCmd;
    delete m_detRadiusCmd;
    delete m_detGeometryCmd;
  }

  void GeometryMessenger::SetNewValue(G4UIcommand* cmd, G4String val) {
    if (cmd == m_detThicknessCmd) {
      G4double parsed_val = m_detThicknessCmd->GetNewDoubleValue(val);
      // This is fine too - just being clear above
      //auto parsed_val = m_detSizeCmd->GetNew3VectorValue(val);
      if(parsed_val< 0.0)
        { G4cerr <<"Error: Thickness must be greater than zero!"<< G4endl; }

      m_dc->set_det_thickness(parsed_val);
      G4cout << "Detector thickness set to " << G4BestUnit(parsed_val, "Length")
        << G4endl;
    }
    if (cmd == m_detRadiusCmd) {
      G4double parsed_val = m_detRadiusCmd->GetNewDoubleValue(val);
      // This is fine too - just being clear above
      //auto parsed_val = m_detSizeCmd->GetNew3VectorValue(val);
      if(parsed_val< 0.0)
        { G4cerr <<"Error: Radius must be greater than zero!"<< G4endl; }

      m_dc->set_det_radius(parsed_val);
      G4cout << "Detector radius set to " << G4BestUnit(parsed_val, "Length")
        << G4endl;
    }
    if (cmd == m_detGeometryCmd) {
      m_dc->set_det_geometry(val);
      G4cout << "Detector geometry set to " << val << G4endl;
    }

    // Command didn't match
    return;
  }
}
