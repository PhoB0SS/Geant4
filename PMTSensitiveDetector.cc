#include "PMTSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SystemOfUnits.hh"
#include "G4Electron.hh"
#include "G4RandomTools.hh"
#include <iostream>

PMTSensitiveDetector::PMTSensitiveDetector(const G4String& name): G4VSensitiveDetector(name), count(0){
    outputFile.open("detector_response.csv");
    outputFile << "n_photons\n";
}

PMTSensitiveDetector::~PMTSensitiveDetector() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

void PMTSensitiveDetector::Initialize(G4HCofThisEvent* hce) {
    // Инициализация на каждом событии
    count = 0;
}

G4bool PMTSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    G4Track* track = aStep->GetTrack();
    
    // Проверяем, что частица является оптическим фотоном
    if (track->GetParticleDefinition()->GetParticleName() == "opticalphoton") {
        // Генерация электрона с квантовой эффективностью 28%
        if (G4UniformRand() < 0.28) {
            ++count; // Увеличиваем счетчик электронов
        }
        
        // Уничтожаем трек фотона
        track->SetTrackStatus(fStopAndKill);
    }

    return true;
}

void PMTSensitiveDetector::EndOfEvent(G4HCofThisEvent* hce) {
    outputFile << count << "\n";
}