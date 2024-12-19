#include "PhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ProcessManager.hh"
#include "G4PhysicsListHelper.hh"
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList() {

    // Электромагнитные процессы
    RegisterPhysics(new G4EmStandardPhysics());

    // Оптические процессы
    RegisterPhysics(new G4OpticalPhysics());

}

PhysicsList::~PhysicsList() {}
