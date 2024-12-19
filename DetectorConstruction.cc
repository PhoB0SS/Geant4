#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "PMTSensitiveDetector.hh"
#include "G4AutoDelete.hh"
#include "G4LogicalVolumeStore.hh"

constexpr G4double goldenRatio() {
    return (1.0 + std::sqrt(5.0)) / 2.0;
}

// Создаем вершины додекаэдра
std::vector<G4ThreeVector> generateDodecahedronVertices(G4double scale) {
    std::vector<G4ThreeVector> vertices;

    G4double phi = goldenRatio();
    G4double invPhi = 1.0 / phi;

    auto addVertex = [&vertices, scale](G4double x, G4double y, G4double z) {
        vertices.emplace_back(scale * x, scale * y, scale * z);
    };

    for (int x : {-1, 1}) {
        for (int y : {-1, 1}) {
            for (int z : {-1, 1}) {
                addVertex(x, y, z);
            }
        }
    }

    for (int i : {-1, 1}) {
        for (int j : {-1, 1}) {
            addVertex(0, i * phi, j * invPhi);
            addVertex(j * invPhi, 0, i * phi);
            addVertex(i * phi, j * invPhi, 0);
        }
    }

    return vertices;
}
DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}

DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {

    G4NistManager* nist = G4NistManager::Instance();
    
    G4double worldSize = 3.0 * m;
    G4Box* worldSolid = new G4Box("World", worldSize / 2, worldSize / 2, worldSize / 2);
    G4Material* worldMaterial = nist->FindOrBuildMaterial("G4_AIR");
    G4LogicalVolume* worldLogical = new G4LogicalVolume(worldSolid, worldMaterial, "World");
    G4VPhysicalVolume* worldPhysical = new G4PVPlacement(
        0,                           // без поворота
        G4ThreeVector(),             // позиция в центре
        worldLogical,                // логический объем
        "World",                     // имя
        nullptr,                     // родительский объем
        false,                       // не требует многократного размещения
        0                            // копия
    );

    std::vector<G4double> photonEnergy = { 3.20 * eV, 3.30 * eV, 3.40 * eV };

    // Определение LAB с добавками PPO и гадолиния
    G4Material* LAB = new G4Material("LAB", 0.86 * g/cm3, 2);
    LAB->AddElement(nist->FindOrBuildElement("C"), 18);
    LAB->AddElement(nist->FindOrBuildElement("H"), 30);
    std::vector<G4double> LAB_RIND = { 1.51, 1.52, 1.53 };
    std::vector<G4double> LAB_ABSL = { 12.0 * m, 12.0 * m, 12 * m };
    G4MaterialPropertiesTable* LABMPT = new G4MaterialPropertiesTable();
    LABMPT->AddProperty("RINDEX", photonEnergy, LAB_RIND);
    LABMPT->AddProperty("ABSLENGTH", photonEnergy, LAB_ABSL);
    LAB->SetMaterialPropertiesTable(LABMPT);

    // Добавка PPO
    G4Material* PPO = new G4Material("PPO", 1.1 * g/cm3, 4);
    PPO->AddElement(nist->FindOrBuildElement("C"), 15);
    PPO->AddElement(nist->FindOrBuildElement("H"), 11);
    PPO->AddElement(nist->FindOrBuildElement("N"), 1);
    PPO->AddElement(nist->FindOrBuildElement("O"), 1);

    // Добавка гадолиния (предположим, GdAcAc3)
    G4Material* GdAcAc3 = new G4Material("GdAcAc3", 1.7 * g/cm3, 4);
    GdAcAc3->AddElement(nist->FindOrBuildElement("Gd"), 1);
    GdAcAc3->AddElement(nist->FindOrBuildElement("C"), 15);
    GdAcAc3->AddElement(nist->FindOrBuildElement("H"), 21);
    GdAcAc3->AddElement(nist->FindOrBuildElement("O"), 6);

    // Определение комбинированного материала сцинтиллятора (LAB + PPO + Gd)
    G4Material* scintillatorMaterial = new G4Material("ScintillatorLAB", 0.86 * g/cm3, 3);
    scintillatorMaterial->AddMaterial(LAB, 99.6 * perCent);
    scintillatorMaterial->AddMaterial(PPO, 0.3 * perCent);
    scintillatorMaterial->AddMaterial(GdAcAc3, 0.1 * perCent);

    // Свойства сцинтиллятора
    G4MaterialPropertiesTable* scintillatorMPT = new G4MaterialPropertiesTable();
    std::vector<G4double> Scintillator_SCINT = { 0.1, 0.8, 0.1 };
    std::vector<G4double> Scintillator_RIND  = { 1.51, 1.52, 1.53 };
    std::vector<G4double> Scintillator_ABSL  = { 5.0 * m, 5.0 * m, 5.0 * m };
    scintillatorMPT->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergy, Scintillator_SCINT);
    scintillatorMPT->AddProperty("RINDEX", photonEnergy, Scintillator_RIND);
    scintillatorMPT->AddProperty("ABSLENGTH", photonEnergy, Scintillator_ABSL);
    scintillatorMPT->AddConstProperty("SCINTILLATIONYIELD", 5000. / MeV);
    scintillatorMPT->AddConstProperty("RESOLUTIONSCALE", 1.0);
    scintillatorMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 0.0 * ns);
    scintillatorMaterial->SetMaterialPropertiesTable(scintillatorMPT);

    // Стальные стенки бака
    G4double cylinderHeight = 1.858 * m;
    G4double cylinderRadius = cylinderHeight / 2;
    G4Material* steel = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    G4double steelThickness = 2 * mm;
    G4Tubs* steelShell = new G4Tubs("SteelShell", 0, cylinderRadius + steelThickness, (cylinderHeight+steelThickness)/2, 0, 2 * M_PI);
    G4LogicalVolume* steelLogical = new G4LogicalVolume(steelShell, steel, "SteelShell");
    new G4PVPlacement(0, G4ThreeVector(), steelLogical, "SteelShell", worldLogical, false, 0);

    // Цилиндрический объём сцинтиллятора без присадок
    G4Tubs* cylinderSolid = new G4Tubs("Cylinder", 0, cylinderRadius, cylinderHeight / 2, 0, 2 * M_PI);
    G4LogicalVolume* cylinderLogical = new G4LogicalVolume(cylinderSolid, LAB, "Cylinder");
    new G4PVPlacement(0, G4ThreeVector(), cylinderLogical, "LABCylinder", steelLogical, false, 0);

    // Сферический объем сцинтиллятора с присадками
    G4double scintillatorRadius = 0.62 * m;
    G4Sphere* scintillatorSolid = new G4Sphere("Scintillator", 0, scintillatorRadius, 0, 2 * M_PI, 0, M_PI);
    G4LogicalVolume* scintillatorLogical = new G4LogicalVolume(scintillatorSolid, scintillatorMaterial, "Scintillator");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), scintillatorLogical, "Scintillator", cylinderLogical, false, 0);

    // Сфера из ПММА вокруг сцинтиллятора с присадками
    G4Material* PMMA = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    std::vector<G4double> PMMA_RIND = { 1.51, 1.52, 1.53 };
    std::vector<G4double> PMMA_ABSL = { 9.0 * m, 9.0 * m, 9.0 * m };
    G4MaterialPropertiesTable* PMMA_mt = new G4MaterialPropertiesTable();
    PMMA_mt->AddProperty("RINDEX", photonEnergy, PMMA_RIND);
    PMMA_mt->AddProperty("ABSLENGTH", photonEnergy, PMMA_ABSL);
    PMMA->SetMaterialPropertiesTable(PMMA_mt);
    G4double PMMAThickness = 10.0 * mm;
    G4Sphere* PMMASolid = new G4Sphere("PMMASphere", scintillatorRadius, scintillatorRadius + PMMAThickness, 0, 2 * M_PI, 0, M_PI);
    G4LogicalVolume* PMMALogical = new G4LogicalVolume(PMMASolid, PMMA, "PMMASphere");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), PMMALogical, "PMMASphere", cylinderLogical, false, 0);

    // Генерация вершин додекаэдра с радиусом размещения
    G4double PMTPlacementRadius = cylinderRadius - 450 * mm;
    std::vector<G4ThreeVector> dodecahedronVertices = generateDodecahedronVertices(PMTPlacementRadius);

    G4double tubeHeight = 166.0 * mm;
    G4double sphereRadius = 202 * mm / 2;

    // Геометрия ФЭУ
    G4Tubs* PMTCylinder = new G4Tubs("PMTCylinder", 0, sphereRadius / 2, tubeHeight / 2, 0, 2 * M_PI);
    G4Sphere* PMTSphere = new G4Sphere("PMTSphere", 0, sphereRadius, 0, 2 * M_PI, 0, M_PI / 2);
    G4UnionSolid* PMTShape = new G4UnionSolid("PMTShape", PMTCylinder, PMTSphere, nullptr, G4ThreeVector(0, 0, tubeHeight / 2));

    G4Material* PMTMaterial = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    std::vector<G4double> PMT_RIND = { 1.51, 1.52, 1.53 };
    std::vector<G4double> PMT_ABSL = { 12.0 * m, 12.0 * m, 12 * m };
    G4MaterialPropertiesTable* PMTPT = new G4MaterialPropertiesTable();
    PMTPT->AddProperty("RINDEX", photonEnergy, PMT_RIND);
    PMTPT->AddProperty("ABSLENGTH", photonEnergy, PMT_ABSL);
    PMTMaterial->SetMaterialPropertiesTable(PMTPT);
    G4LogicalVolume* PMTLogical = new G4LogicalVolume(PMTShape, PMTMaterial, "PMT");

    // Размещение ФЭУ
    for (size_t i = 0; i < dodecahedronVertices.size(); ++i) {
        G4ThreeVector position = dodecahedronVertices[i];
        G4ThreeVector direction = -position.unit();

        // Выбираем ось Z как направляющую ось ФЭУ
        G4ThreeVector zAxis = direction;
        G4ThreeVector xAxis = G4ThreeVector(1, 0, 0).cross(zAxis).unit();
        G4ThreeVector yAxis = zAxis.cross(xAxis);

        // Создаём матрицу вращения
        G4RotationMatrix* rotation = new G4RotationMatrix();
        rotation->rotateAxes(xAxis, yAxis, zAxis);

        // Размещаем ФЭУ
        new G4PVPlacement(G4Transform3D(*rotation, position), PMTLogical, "PMT", cylinderLogical, false, i);
        G4AutoDelete::Register(rotation);
    }

    return worldPhysical;
}

void DetectorConstruction::ConstructSDandField() {
    // Создаём чувствительный детектор
    auto* pmtSD = new PMTSensitiveDetector("PMTSD");

    // Регистрируем детектор в G4SDManager
    G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);

    // Получаем логический объем для PMT, который был создан ранее
    G4LogicalVolumeStore::GetInstance()->GetVolume("PMT")->SetSensitiveDetector(pmtSD);
}