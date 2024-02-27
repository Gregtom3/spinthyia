#include "DISTree.h"
#include <iostream>

using namespace std;

DISTree::DISTree(const std::string& filename, HadroniumAnalysisType analysisType) {
    this->init(filename, analysisType);
}


void DISTree::init(const std::string& filename, HadroniumAnalysisType analysisType){
    
    file = new TFile(filename.data(), "RECREATE");
    tree = new TTree("tree", "Kinematics Data Tree");

    // Branches for EventKinematics are always created
    tree->Branch("x", &this->eventKinematics->x, "x/D");
    tree->Branch("y", &this->eventKinematics->y, "y/D");
    tree->Branch("Q2", &this->eventKinematics->Q2, "Q2/D");
    tree->Branch("W", &this->eventKinematics->W, "W/D");
    tree->Branch("beam_polarization", &this->eventKinematics->beam_polarization, "bPol/I");
    tree->Branch("target_polarization", &this->eventKinematics->target_polarization, "tPol/I");
    
    // Branches for SingleHadronKinematics
    if (analysisType == HadroniumAnalysisType::SingleHadron) {
        doSingleHadron = true;
        tree->Branch("pt", &this->singleHadronKinematics->pt, "pt/D");
        tree->Branch("z", &this->singleHadronKinematics->z, "z/D");
        tree->Branch("phi", &this->singleHadronKinematics->phi, "phi/D");
        tree->Branch("Mh", &this->singleHadronKinematics->Mh, "Mh/D");
        tree->Branch("xF", &this->singleHadronKinematics->xF, "xF/D");
        tree->Branch("Mx", &this->singleHadronKinematics->Mx, "Mx/D");
    }

    // Branches for DiHadronKinematics
    if (analysisType == HadroniumAnalysisType::DiHadron) {
        doDiHadron = true;
        tree->Branch("pt1", &this->diHadronKinematics->pt1, "pt1/D");
        tree->Branch("pt2", &this->diHadronKinematics->pt2, "pt2/D");
        tree->Branch("pt", &this->diHadronKinematics->pt, "pt/D");
        tree->Branch("z1", &this->diHadronKinematics->z1, "z1/D");
        tree->Branch("z2", &this->diHadronKinematics->z2, "z2/D");
        tree->Branch("z", &this->diHadronKinematics->z, "z/D");
        tree->Branch("phi_h", &this->diHadronKinematics->phi_h, "phi_h/D");
        tree->Branch("phi_RT", &this->diHadronKinematics->phi_RT, "phi_RT/D");
        tree->Branch("phi_Rperp", &this->diHadronKinematics->phi_Rperp, "phi_Rperp/D");
        tree->Branch("Mh", &this->diHadronKinematics->Mh, "Mh/D");
        tree->Branch("xF1", &this->diHadronKinematics->xF1, "xF1/D");
        tree->Branch("xF2", &this->diHadronKinematics->xF2, "xF2/D");
        tree->Branch("xF", &this->diHadronKinematics->xF, "xF/D");
        tree->Branch("Mx", &this->diHadronKinematics->Mx, "Mx/D");
    }
}

void DISTree::Fill(LundEvent& event, std::vector<std::vector<Hadronium>> hadronia) {
    
    // Create kinematics object for the event
    KinematicsCalculator kin(event);
    
    // Get the event kinematics
    *eventKinematics = kin.CalculateEventKinematics();
    
    if (doSingleHadron){
        // Get the single hadron kinematics for all event hadronia
        std::vector<SingleHadronKinematics> single_hadron_kins = kin.CalculateSingleHadronKinematics(hadronia);
        
        for (auto& single_hadron_kin : single_hadron_kins){
            *singleHadronKinematics = single_hadron_kin;
            tree->Fill();
        }
    }
    else if (doDiHadron){
        // Get the dihadron kinematics for all event hadronia
        std::vector<DiHadronKinematics> dihadron_kins = kin.CalculateDiHadronKinematics(hadronia);
        for (auto& dihadron_kin : dihadron_kins){
            *diHadronKinematics = dihadron_kin;
            tree->Fill();
        }
    }
    else{
        tree->Fill();
    }
}

void DISTree::Write() {
    file->WriteTObject(tree);
    file->Close();
}

DISTree::~DISTree() {
    delete file; // Ensure proper cleanup
}