#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction() : G4UserRunAction() {}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run* run) {
    G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run) {
    G4cout << "### Run " << run->GetRunID() << " end." << G4endl;
}
