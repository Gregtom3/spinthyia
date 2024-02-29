#include "LundAnalysis.h"

int example_A_single_pion(std::string pattern = "out/tmp6/gen/pythia8/",
                          std::string output_file = "example_A_out.root") {
    
    LundAnalysis analysis(pattern, output_file, HadroniumAnalysisType::SingleHadron, 1);
    analysis.setCriteria("(211)");
    analysis.addKinematicCut(KinematicCut("Q2", KinematicCut::CutType::MIN, 1.0)); // Q2 > 1
    analysis.run();
    return 0;
}