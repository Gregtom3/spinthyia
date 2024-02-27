#ifndef LUNDANALYSIS_H
#define LUNDANALYSIS_H

#include "LundReader.h"
#include "HadroniumParser.h"
#include "HadroniaFilter.h"
#include "Kinematics.h"
#include "KinematicsStructs.h"
#include "DISTree.h"
#include <string>
#include <vector>

class LundAnalysis {
public:
    LundAnalysis(const std::string& filename, const std::string& outputFilename, HadroniumAnalysisType analysisType);
    LundAnalysis(const std::string& filename, const std::string& outputFilename, HadroniumAnalysisType analysisType, int verbosity);
    void setCriteria(const std::string& criteria);
    void setFilterRules(const FilterRules& rules);
    void run();

private:
    int numPassed = 0;
    int eventCount = 0;
    int verbosity;
    DISTree distree;
    std::string filename;
    std::string outputFilename;
    std::string criteria;
    HadroniumAnalysisType analysisType;
    FilterRules rules;
    void processEvent(LundEvent& event);
};

#endif // LUNDANALYSIS_H
