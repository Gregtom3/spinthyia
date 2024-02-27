#include "LundAnalysis.h"
//#include "Riostream.h"

R__LOAD_LIBRARY(Spinthyia)

LundAnalysis::LundAnalysis(const std::string& pattern, const std::string& outputFilename, HadroniumAnalysisType analysisType, int verbosity)
: outputFilename(outputFilename), analysisType(analysisType), verbosity(verbosity) {
    if (fs::is_directory(pattern)) {
        for (const auto& entry : fs::directory_iterator(pattern))
            if (!fs::is_directory(entry.path())) // Avoid adding directories
                filenames.push_back(entry.path().string());
    } else {
        filenames.push_back(pattern); // Assuming a single file
    }
    // Initialize distree once, assuming same outputFilename and analysisType for all files
    distree.init(outputFilename, analysisType);
}

void LundAnalysis::setCriteria(const std::string& criteria) {
    this->criteria = criteria;
}

void LundAnalysis::setFilterRules(const FilterRules& rules) {
    this->rules = rules;
}

void LundAnalysis::addKinematicCut(const KinematicCut& cut) {
    distree.kinematicCuts.push_back(cut);
}

void LundAnalysis::run() {
    for (const auto& file : filenames) {
        LundReader reader(file);
        LundEvent event;
        while (reader.readEvent(event)) {
            processEvent(event);
            eventCount++;
            if (eventCount % 10000 == 0 && verbosity > 0) {
                std::cout << "Processed " << eventCount << " events from " << file << std::endl;
            }
        }
    }
    distree.Write();
}

void LundAnalysis::processEvent(LundEvent& event) {
    std::vector<std::vector<Hadronium>> hadronia = reconstruct_hadronia(event, criteria);
    if (!rules.isEmpty()) {
        hadronia = filterHadronia(hadronia, rules);
    }
    if (hadronia.empty()) return;
    distree.Fill(event, hadronia);
    if (numPassed < 20 && verbosity > 0)
        printHadronia(hadronia);
    numPassed++;
}
