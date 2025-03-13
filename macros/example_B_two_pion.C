#include "LundAnalysis.h"

int example_B_two_pion(std::string pattern = "./string*.root",
                       //std::string pattern = "out/test_parallel_v3/gen/pythia8/stringspinner.pythia8.gemc.lund.LU.1.0000.dat",
                       std::string output_file = "example_B_out.root") {
    
    // Initialize LundAnalysis with the specific filename, output file, and kinematics settings
    LundAnalysis analysis(pattern, output_file, HadroniumAnalysisType::DiHadron, 1);
    
    // Set the criteria for selecting two pions
    analysis.setCriteria("(211) + (-211)");
    
    // Add a Mx cut
    analysis.addKinematicCut(KinematicCut("Mx", KinematicCut::CutType::MIN, 1.5)); // Mx > 1.5
    
    // Add an acceptance for final state particles
    analysis.setCLAS12();
    
    // Run the analysis
    analysis.run();
    
    return 0;
}