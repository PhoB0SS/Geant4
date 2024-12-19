#ifndef PMTSENSITIVEDETECTOR_HH
#define PMTSENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "globals.hh"
#include <fstream>
#include <iomanip> 

class PMTSensitiveDetector : public G4VSensitiveDetector {
public:
    // Конструктор
    PMTSensitiveDetector(const G4String& name);

    // Деструктор
    ~PMTSensitiveDetector() override;

    // Метод инициализации на событие
    void Initialize(G4HCofThisEvent* hce) override;

    // Метод обработки "хита" (шаг внутри чувствительного объема)
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;

    // Метод завершения обработки события
    void EndOfEvent(G4HCofThisEvent* hce) override;

    std::ofstream outputFile;

private:
    G4int count;
    G4ThreeVector interactionPosition;
};

#endif
