#ifndef DISTREE_H
#define DISTREE_H

#include "TFile.h"
#include "TTree.h"
#include "LundReader.h"
#include "Kinematics.h"
#include "KinematicsStructs.h"
#include <memory>
#include <vector>

class DISTree {
public:
    // Constructor with flags to indicate whether to record single and di-hadron kinematics
    DISTree(const char* filename, bool do_single_hadron = false, bool do_dihadron = false);
    ~DISTree();

    void SetEventKinematics(const EventKinematics& ek);
    void SetSingleHadronKinematics(const std::vector<SingleHadronKinematics>& shk);
    void SetDiHadronKinematics(const std::vector<DiHadronKinematics>& dhk);

    void Fill(LundEvent& event, std::vector<std::vector<Hadronium>> hadronia);
    void Write();

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