#include "LundAnalysis.h"
#include "Riostream.h"

R__LOAD_LIBRARY(Spinthyia)
    
LundAnalysis::LundAnalysis(const std::string& pattern, const std::string& outputFilename, HadroniumAnalysisType analysisType, int verbosity)
: outputFilename(outputFilename), analysisType(analysisType), verbosity(verbosity) {
    // Get the std::vector<string> filenames (capable of handling wildcards)
    filenames = findMatchingFiles(pattern);
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

void LundAnalysis::setCLAS12() {
    acc = AcceptanceType::CLAS12;
}

void LundAnalysis::processEvent(LundEvent& event) {
    std::vector<std::vector<Hadronium>> hadronia = reconstruct_hadronia(event, criteria, acc);
    if (!rules.isEmpty()) {
        hadronia = filterHadronia(hadronia, rules);
    }
    if (hadronia.empty()) return;
    distree.Fill(event, hadronia);
    if (numPassed < 20 && verbosity > 0)
        printHadronia(hadronia);
    numPassed++;
}

std::vector<std::string> LundAnalysis::findMatchingFiles(const std::string& pattern) {
    // Split the pattern into directory path and file pattern
    fs::path patternPath(pattern);
    fs::path dirPath = patternPath.parent_path();
    std::string filenamePattern = patternPath.filename().string();

    // Convert wildcard pattern to regex
    // This is a simple conversion: replace '*' with ".*"
    std::string regexPattern = std::regex_replace(filenamePattern, std::regex("\\*"), ".*");
    std::regex finalPattern(regexPattern, std::regex::basic);

    std::vector<std::string> matchingFiles;

    // Check if directory exists
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        std::cerr << "Directory does not exist: " << dirPath << std::endl;
        return matchingFiles;
    }

    // Iterate over files in the directory and match against the pattern
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (fs::is_regular_file(entry) && std::regex_match(entry.path().filename().string(), finalPattern)) {
            matchingFiles.push_back(entry.path().string());
        }
    }

    return matchingFiles;
}