#ifndef DISTREE_H
#define DISTREE_H

#include "TFile.h"
#include "TTree.h"
#include "LundReader.h"
#include "Kinematics.h"
#include "KinematicsStructs.h"
#include "KinematicCut.h"
#include <memory>
#include <vector>

class DISTree {
public:
    DISTree(){};
    DISTree(const std::string& filename, HadroniumAnalysisType analysisType);
    ~DISTree();

    void init(const std::string& filename, HadroniumAnalysisType analysisType);
    void SetEventKinematics(const EventKinematics& ek);
    void SetSingleHadronKinematics(const std::vector<SingleHadronKinematics>& shk);
    void SetDiHadronKinematics(const std::vector<DiHadronKinematics>& dhk);

    void Fill(LundEvent& event, std::vector<std::vector<Hadronium>> hadronia);
    bool checkCuts() const;
    void Write();

    std::vector<KinematicCut> kinematicCuts;
    
private:
    TFile* file;
    TTree* tree;

    EventKinematics* eventKinematics = new EventKinematics;
    SingleHadronKinematics* singleHadronKinematics = new SingleHadronKinematics;
    DiHadronKinematics* diHadronKinematics = new DiHadronKinematics;

    
    bool doSingleHadron = false;
    bool doDiHadron = false;
};

#endif // DISTREE_H