#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv) {
    G4UIExecutive* ui = nullptr;
    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    #ifdef G4MULTITHREADED
        G4RunManager* runManager = new G4RunManager();
    #else
        G4MTRunManager* runManager = new G4MTRunManager();
    #endif

    // Инициализация компонентов Geant4
    runManager->SetUserInitialization(new DetectorConstruction);
    runManager->SetUserInitialization(new PhysicsList);
    runManager->SetUserInitialization(new ActionInitialization);

    // Инициализация визуализации
    G4VisExecutive* visManager = new G4VisExecutive();
    visManager->Initialize();

    G4int nEvents = 100;
    runManager->BeamOn(nEvents);

    // Инициализация интерфейса управления
    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/control/execute /home/phoboss/Documents/Geant4/detector_simulation/build/init_vis.mac");
    ui->SessionStart();
    delete ui;

    runManager->Initialize();

    delete visManager;
    delete runManager;
    return 0;
}
