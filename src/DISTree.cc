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
            if (checkCuts()==true) tree->Fill();
        }
    }
    else if (doDiHadron){
        // Get the dihadron kinematics for all event hadronia
        std::vector<DiHadronKinematics> dihadron_kins = kin.CalculateDiHadronKinematics(hadronia);
        for (auto& dihadron_kin : dihadron_kins){
            *diHadronKinematics = dihadron_kin;
            if (checkCuts()==true) tree->Fill();
        }
    }
    else{
        if (checkCuts()==true) tree->Fill();
    }
}


bool DISTree::checkCuts() const {
    for (const auto& cut : kinematicCuts) {
        double value = 0.0;

        // EventKinematics
        if (cut.variableName == "Q2") value = eventKinematics->Q2;
        else if (cut.variableName == "x") value = eventKinematics->x;
        else if (cut.variableName == "y") value = eventKinematics->y;
        else if (cut.variableName == "W") value = eventKinematics->W;

        // SingleHadronKinematics
        else if (cut.variableName == "pt" && doSingleHadron) value = singleHadronKinematics->pt;
        else if (cut.variableName == "z" && doSingleHadron) value = singleHadronKinematics->z;
        else if (cut.variableName == "phi" && doSingleHadron) value = singleHadronKinematics->phi;
        else if (cut.variableName == "Mh" && doSingleHadron) value = singleHadronKinematics->Mh;
        else if (cut.variableName == "xF" && doSingleHadron) value = singleHadronKinematics->xF;
        else if (cut.variableName == "Mx" && doSingleHadron) value = singleHadronKinematics->Mx;

        // DiHadronKinematics
        else if (cut.variableName == "pt1" && doDiHadron) value = diHadronKinematics->pt1;
        else if (cut.variableName == "pt2" && doDiHadron) value = diHadronKinematics->pt2;
        else if (cut.variableName == "pt" && doDiHadron) value = diHadronKinematics->pt;
        else if (cut.variableName == "z1" && doDiHadron) value = diHadronKinematics->z1;
        else if (cut.variableName == "z2" && doDiHadron) value = diHadronKinematics->z2;
        else if (cut.variableName == "z" && doDiHadron) value = diHadronKinematics->z;
        else if (cut.variableName == "phi_h" && doDiHadron) value = diHadronKinematics->phi_h;
        else if (cut.variableName == "phi_RT" && doDiHadron) value = diHadronKinematics->phi_RT;
        else if (cut.variableName == "phi_Rperp" && doDiHadron) value = diHadronKinematics->phi_Rperp;
        else if (cut.variableName == "xF1" && doDiHadron) value = diHadronKinematics->xF1;
        else if (cut.variableName == "xF2" && doDiHadron) value = diHadronKinematics->xF2;
        else if (cut.variableName == "xF" && doDiHadron) value = diHadronKinematics->xF;
        else if (cut.variableName == "Mx" && doDiHadron) value = diHadronKinematics->Mx;
        
        switch (cut.type) {
            case KinematicCut::CutType::MIN:
                if (value < cut.minValue) return false;
                break;
            case KinematicCut::CutType::MAX:
                if (value > cut.maxValue) return false;
                break;
            case KinematicCut::CutType::RANGE:
                if (value < cut.minValue || value > cut.maxValue) return false;
                break;
        }
    }
    return true;
}

void DISTree::Write() {
    file->WriteTObject(tree);
    file->Close();
}

DISTree::~DISTree() {
    delete file; // Ensure proper cleanup
}