#ifndef LUNDANALYSIS_H
#define LUNDANALYSIS_H

#include "LundReader.h"
#include "HadroniumParser.h"
#include "HadroniaFilter.h"
#include "Kinematics.h"
#include "KinematicsStructs.h"
#include "KinematicCut.h"
#include "DISTree.h"
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

class LundAnalysis {
public:
    LundAnalysis(const std::string& pattern, const std::string& outputFilename, HadroniumAnalysisType analysisType, int verbosity);
    void setCriteria(const std::string& criteria);
    void setFilterRules(const FilterRules& rules);
    void addKinematicCut(const KinematicCut& cut);
    void run();
    void setCLAS12();
    
private:
    int numPassed = 0;
    int eventCount = 0;
    int verbosity;
    DISTree distree;
    std::vector<std::string> filenames;
    std::string outputFilename;
    std::string criteria;
    HadroniumAnalysisType analysisType;
    FilterRules rules;
    AcceptanceType acc = AcceptanceType::ALL;
    void processEvent(LundEvent& event);
    std::vector<std::string> findMatchingFiles(const std::string& pattern);
};

#endif // LUNDANALYSIS_H
