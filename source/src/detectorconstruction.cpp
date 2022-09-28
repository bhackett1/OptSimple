#include "detectorconstruction.hpp"
#include "globals.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "sensitivedetector.hpp"
#include "G4SDManager.hh"
#include "geometrymessenger.hpp"
#include "materialmessenger.hpp"
#include "G4TessellatedSolid.hh"
#include "CADMesh.hh"


namespace ne697 {
  DetectorConstruction::DetectorConstruction():
    G4VUserDetectorConstruction(),
    m_trackingVols(),
    m_detThickness(5.*cm),
    m_detRadius(50.*cm),
    m_detMaterial("G4_AIR"),
    m_worldMaterial("G4_SODIUM_IODIDE"),
    m_detGeometry("Cylinder")
    {
      G4cout << "Creating DetectorConstruction" << G4endl;
      m_gmessenger = new GeometryMessenger(this);
      m_mmessenger = new MaterialMessenger(this);
      build_materials();
    }

    DetectorConstruction::~DetectorConstruction() {
     delete m_gmessenger;
     delete m_mmessenger;
      G4cout << "Deleting DetectorConstruction" << G4endl;
    }

  G4PVPlacement* DetectorConstruction::Construct() {
    auto world_solid = new G4Box("world_solid", m_detRadius*2., m_detRadius*2., m_detRadius*2.);
    auto nist = G4NistManager::Instance();
    // auto world_mat = nist->FindOrBuildMaterial("G4_AIR");
    // auto world_mat = liq_Ar;
    // auto world_mat = nist->FindOrBuildMaterial("NE697_LIQUID_AR");
    auto world_mat = nist->FindOrBuildMaterial(m_worldMaterial);

    auto world_log = new G4LogicalVolume(
        world_solid,
        world_mat,
        "world_log"
    );
    // Commented since we don't want to record Hits in the world's air volume
    //m_trackingVols.push_back(world_log);
    auto world_phys = new G4PVPlacement(
        nullptr,
        G4ThreeVector(0, 0, 0),
        world_log,
        "world_phys",
        nullptr,
        false,
        0,
        true
    );


    //Create PEN shape
    //Import CAD Shape
    auto PEN_mesh = CADMesh::TessellatedMesh::FromSTL("./Capsule.stl");

    //Define PEN material
    auto PEN_mat = nist->FindOrBuildMaterial("NE697_PEN");
    //Logic for PEN plate

    auto rotation = new G4RotationMatrix();
    rotation->rotateX(90*deg);

    auto PEN_logic = new G4LogicalVolume(PEN_mesh->GetSolid(),PEN_mat,"PEN_logic");
    new G4PVPlacement( rotation,
			G4ThreeVector(0*cm,0*cm,-5*cm),
			PEN_logic,
			"PEN_phys",
			world_log,
			false,
			0,
			true);

    auto HPGE_mat = nist->FindOrBuildMaterial("G4_Ge");

    auto solidHPGE = new G4Tubs("solidHPGE",
                                0.*mm,
                                24.6*mm,
                                23.5*mm,
                                0.0*rad,CLHEP::twopi*rad);

    auto logicHPGE = new G4LogicalVolume(solidHPGE, HPGE_mat, "logicHPGE");
    m_trackingVols.push_back(logicHPGE);
    new G4PVPlacement(
      nullptr,
      G4ThreeVector(0, 0.*cm, 0*cm),
      logicHPGE,
      "physHPGE",
      world_log,
      false,
      0,
      true
    );


    auto det_mat = nist->FindOrBuildMaterial(m_detMaterial);

    if(m_detGeometry == "Cylinder")
    {
      auto det_solidCylinder = new G4Tubs("det_solidCylinder",
                                           m_detRadius,
                                           m_detRadius+0.5*cm,
                                           m_detRadius,
                                           0.0*rad,CLHEP::twopi*rad);

      auto det_log = new G4LogicalVolume(det_solidCylinder, det_mat, "det_log");
      m_trackingVols.push_back(det_log);
      new G4PVPlacement(
        nullptr,
        G4ThreeVector(0*cm, 0.*cm, 0*cm),
        det_log,
        "det_phys",
        world_log,
        false,
        0,
        true
      );
    }
    else
    {
      auto det_solidSphere = new G4Sphere("det_solidSphere",
                                    m_detRadius,
                                    m_detRadius+0.5*cm,
                                    0.0*rad, CLHEP::twopi*rad,
                                    0.0*rad, CLHEP::pi*rad);

      auto det_log = new G4LogicalVolume(det_solidSphere, det_mat, "det_log");
      m_trackingVols.push_back(det_log);
      new G4PVPlacement(
        nullptr,
        G4ThreeVector(0, 0.*cm, 0*cm),
        det_log,
        "det_phys",
        world_log,
        false,
        0,
        true
      );
    }

    return world_phys;
  }

  void DetectorConstruction::ConstructSDandField() {
    // We will ask for "world_sd_hits" later in Run::RecordEvent()
    auto sd = new SensitiveDetector("world_sd");
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    // Connect the sensitive detector to all of the logical volumes on the list
    for (auto& log : m_trackingVols) {
      SetSensitiveDetector(log, sd);
    }
    return;
  }

  void DetectorConstruction::build_materials() {


    //Build PEN material:
    //------------------------------------------------------------------------//
    G4Element* PEN_H = new G4Element("Hydrogen", "H", 1., 1.01*g/mole);
    G4Element* PEN_C = new G4Element("Carbon",   "C", 6., 12.01*g/mole);
    G4Element* PEN_O = new G4Element("Oxygen",   "O", 8., 16.01*g/mole);

    auto PEN = new G4Material("NE697_PEN", 1.36*g/cm3, 3);
    PEN->AddElement(PEN_H, 10);
    PEN->AddElement(PEN_C, 14);
    PEN->AddElement(PEN_O, 4);
    G4cout << "PEN MATERIAL: " << PEN << G4endl;

    auto PEN_mpt = new G4MaterialPropertiesTable;
    double wavelengthPEN [10] = {200,300,400,420,440,460,480,500,600,700};
    double energiesPEN [10];
    double temp;
    for (unsigned int a = 0; a <10;a++)
    {
      temp = (1239.8/wavelengthPEN[a]);
      energiesPEN[a] = temp*eV;
    }
    //Characterization of biaxially-stretched plastic films bygeneralized ellipsometry, 1998
    double rindexPEN[10] = { 1.8, 	1.7, 1.69, 1.68, 1.67,
                             1.67, 1.66, 1.66, 1.64, 1.63};
    //B. Hackett, 2021
    double abslengthPEN[10] = { 0.03*cm, 0.03*cm, 0.35*cm, 3.*cm, 5.45*cm, 6.61*cm,
                               7.08*cm, 7.15*cm, 7.76*cm, 19.45*cm};
    //
    double emissionPEN[10] = { 0.0, 0.0, 0.0145, 0.04221, 0.0582, 0.0542, 0.0384,
                               0.02381, 0.00056, 8.23e-5};
    PEN_mpt->AddProperty("RINDEX", energiesPEN, rindexPEN, 10);
    PEN_mpt->AddProperty("ABSLENGTH", energiesPEN, abslengthPEN, 10);
    PEN_mpt->AddProperty("SCINTILLATIONCOMPONENT1", energiesPEN, emissionPEN, 10);
    // PEN_mpt->AddConstProperty("YIELDRATIO", 1.);
    PEN_mpt->AddProperty("WLSABSLENGTH",energiesPEN, abslengthPEN, 10);
    PEN_mpt->AddProperty("WLSCOMPONENT", energiesPEN, emissionPEN, 10);
    PEN_mpt->AddConstProperty("WLSTIMECONSTANT", 0.5 * ns);
    //PEN Collaboration 2021
    PEN_mpt->AddConstProperty("SCINTILLATIONYIELD", 5500./MeV);
    PEN->SetMaterialPropertiesTable(PEN_mpt);
    //------------------------------------------------------------------------//


    //Build liquid argon material:
    //------------------------------------------------------------------------//
    auto Ar40 = new G4Isotope("LAr-Ar40", 18, 40, 39.962*g/mole);
    auto Ar38 = new G4Isotope("LAr-Ar38", 18, 38, 37.9627*g/mole);
    auto Ar36 = new G4Isotope("LAr-Ar36", 18, 36, 35.9675*g/mole);

    auto nat_Ar = new G4Element("natAr", "natAr", 3);
    nat_Ar->AddIsotope(Ar40, 99.604*perCent);
    nat_Ar->AddIsotope(Ar38, 0.063*perCent);
    nat_Ar->AddIsotope(Ar36, 0.333*perCent);
    //

    auto liq_Ar = new G4Material("LIQUID_AR", 1.3982*g/cm3, 1);
    liq_Ar->AddElement(nat_Ar, 1);
    G4cout << "Liquid Argon MATERIAL: " << liq_Ar << G4endl;

    auto nist_man = G4NistManager::Instance();
    auto lAr_mpt = new G4MaterialPropertiesTable;
    double wavelengthLAr [10] = {122.4,127.9,133.3,138.7,144.2,155,160.4,209,
                                 350.5,459.1};
    double energiesLAr [10];
    for (unsigned int a = 0; a <10;a++)
    {
      temp = (1239.8/wavelengthLAr[a]);
      energiesLAr[a] = temp*eV;
    }
    //Grace et al 2017
    double rindexLAr[10] = { 1.48738, 	1.418878, 1.37767, 1.349766, 1.329322,
                             1.30244, 1.293009, 1.253168, 1.230057, 1.2258};
    // //Attenuation of vacuum ultraviolet light in liquid argon, A. Neumeier et al, 2012
    double abslengthLAr[10] = { 45.8*cm, 161.8*cm, 511.*cm, 4500.*cm, 10*m, 20*m,
                               100*m, 100*m, 100*m, 100*m};
    //The scintillation of liquid argon, T. Heindl et al, 2010
    double emissionLAr[10] = { 1688., 3964, 782.3, 11.7, 5.8, 0., 23.5, 0., 0.,
                               0. };
    lAr_mpt->AddProperty("RINDEX", energiesLAr, rindexLAr, 10);
    lAr_mpt->AddProperty("ABSLENGTH", energiesLAr, abslengthLAr, 10);
    lAr_mpt->AddProperty("SCINTILLATIONCOMPONENT1", energiesLAr, emissionLAr, 10);
    lAr_mpt->AddConstProperty("YIELDRATIO", 1.);
    //Predicting transport effects of scintillation light signals in large-scale liquid argon detectors, 2021
    lAr_mpt->AddConstProperty("SCINTILLATIONYIELD", 40000./MeV);
    liq_Ar->SetMaterialPropertiesTable(lAr_mpt);
    //------------------------------------------------------------------------//



    // Sodium iodide
    G4Material* nai = nist_man->FindOrBuildMaterial("G4_SODIUM_IODIDE");
    auto nai_mpt = new G4MaterialPropertiesTable;
    double energies[2] = { 2*eV, 5*eV };
    double rindex[2] = { 1.85, 1.85 };
    double abslength[2] = { 1000.*m, 1000.*m };
    nai_mpt->AddProperty("RINDEX", energies, rindex, 2);
    nai_mpt->AddProperty("ABSLENGTH", energies, abslength, 2);

    // NaI scintillation
    double emission[2] = { 1.0, 1.0 };
    nai_mpt->AddProperty("SCINTILLATIONCOMPONENT1", energies, emission, 2);
    nai_mpt->AddConstProperty("SCINTILLATIONYIELD", 38./MeV);
    nai_mpt->AddConstProperty("FASTTIMECONSTANT", 250.*ns);
    nai_mpt->AddConstProperty("FASTSCINTILLATIONRISETIME", 0.5*us);
    nai_mpt->AddConstProperty("YIELDRATIO", 1.);
    nai_mpt->AddConstProperty("RESOLUTIONSCALE", 1.);

    nai->SetMaterialPropertiesTable(nai_mpt);

    return;
  }

  void DetectorConstruction::set_det_thickness(G4double const& detthickness) {
    m_detThickness = detthickness;
    return;
  }

  G4double const& DetectorConstruction::get_det_thickness() const {
    return m_detThickness;
  }

  void DetectorConstruction::set_det_radius(G4double const& detradius) {
    m_detRadius = detradius;
    return;
  }

  G4double const& DetectorConstruction::get_det_radius() const {
    return m_detRadius;
  }

  G4String const& DetectorConstruction::get_det_material() const {
    return m_detMaterial;
  }

  void DetectorConstruction::set_det_material(G4String const& material) {
    m_detMaterial = material;
    return;
  }

  G4String const& DetectorConstruction::get_world_material() const {
    return m_worldMaterial;
  }

  void DetectorConstruction::set_world_material(G4String const& material) {
    m_worldMaterial = material;
    return;
  }

  G4String const& DetectorConstruction::get_det_geometry() const {
    return m_detGeometry;
  }

  void DetectorConstruction::set_det_geometry(G4String const& geometry) {
    m_detGeometry = geometry;
    return;
  }
}
