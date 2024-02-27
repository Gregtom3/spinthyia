#include "LundAnalysis.h"
#include "Riostream.h"

R__LOAD_LIBRARY(Spinthyia)

LundAnalysis::LundAnalysis(const std::string& filename, const std::string& outputFilename, HadroniumAnalysisType analysisType)
: filename(filename), outputFilename(outputFilename), analysisType(analysisType), verbosity(1) {
}

LundAnalysis::LundAnalysis(const std::string& filename, const std::string& outputFilename, HadroniumAnalysisType analysisType, int verbosity)
: filename(filename), outputFilename(outputFilename), analysisType(analysisType), verbosity(verbosity) {
}

void LundAnalysis::setCriteria(const std::string& criteria) {
    this->criteria = criteria;
}

void LundAnalysis::setFilterRules(const FilterRules& rules) {
    this->rules = rules;
}

void LundAnalysis::run() {
    distree.init(outputFilename, analysisType);
    LundReader reader(filename);
    LundEvent event;
    while (reader.readEvent(event)) {
        processEvent(event);
        eventCount++;
        if (eventCount % 10000 == 0 && verbosity > 0) {
            std::cout << "Processed " << eventCount << " events." << std::endl;
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
