#include "LundAnalysis.h"

int example_B_two_pion() {
    // Initialize LundAnalysis with the specific filename, output file, and kinematics settings
    LundAnalysis analysis("out/tmp/gen/stringspinner.pythia8.gemc.lund.LU.1.0000.dat", "example_B_out.root", HadroniumAnalysisType::DiHadron);
    
    // Set the criteria for selecting two pions
    analysis.setCriteria("(211) + (-211)");
    
    // Run the analysis
    analysis.run();
    
    return 0;
}