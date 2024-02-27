#include "LundAnalysis.h"

int example_B_two_pion(std::string pattern = "out/tmp2/gen/pythia8/",
                       std::string output_file = "example_B_out.root") {
    
    // Initialize LundAnalysis with the specific filename, output file, and kinematics settings
    LundAnalysis analysis(pattern, output_file, HadroniumAnalysisType::DiHadron, 1);
    
    // Set the criteria for selecting two pions
    analysis.setCriteria("(211) + (-211)");
    
    // Add a Mx cut
    analysis.addKinematicCut(KinematicCut("Mx", KinematicCut::CutType::MIN, 1.5)); // Mx > 1.5
    
    // Run the analysis
    analysis.run();
    
    return 0;
}