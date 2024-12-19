#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4RandomTools.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction() {
    G4int n_particles = 1;
    fParticleGun = new G4ParticleGun(n_particles);

    // Выбираем частицу, например, электрон
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "e+";
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
    fParticleGun->SetParticleDefinition(particle);
    outputFile.open("particle_birth.csv");
    outputFile << "x,y,z\n";
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    G4double scintillatorRadius = 0.62 * m;

    // Генерация случайной точки внутри сцинтиллятора
    G4ThreeVector randomPosition = GenerateRandomPointInSphere(scintillatorRadius);

    // Установка позиции частицы
    fParticleGun->SetParticlePosition(randomPosition);

    // Установка изотропного направления
    G4ThreeVector direction = GenerateRandomPointInSphere(1.0).unit();
    fParticleGun->SetParticleMomentumDirection(direction);

    // Установка энергии частицы
    fParticleGun->SetParticleEnergy(3.0 * MeV);

    // Генерация частицы
    fParticleGun->GeneratePrimaryVertex(event);

    outputFile << randomPosition.x() / CLHEP::m << "," << randomPosition.y() / CLHEP::m << "," << randomPosition.z() / CLHEP::m << "\n";
}

G4ThreeVector PrimaryGeneratorAction::GenerateRandomPointInSphere(G4double radius) {
    G4double r = radius * std::cbrt(G4UniformRand());
    G4double theta = std::acos(1 - 2 * G4UniformRand());
    G4double phi = 2 * CLHEP::pi * G4UniformRand();

    G4double x = r * std::sin(theta) * std::cos(phi);
    G4double y = r * std::sin(theta) * std::sin(phi);
    G4double z = r * std::cos(theta);

    return G4ThreeVector(x, y, z);
}
