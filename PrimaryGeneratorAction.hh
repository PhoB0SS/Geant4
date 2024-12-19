#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4Event.hh"
#include <fstream>

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event* event) override;
    std::ofstream outputFile;

private:
    G4ParticleGun* fParticleGun;
    static G4ThreeVector GenerateRandomPointInSphere(G4double radius);
};

#endif