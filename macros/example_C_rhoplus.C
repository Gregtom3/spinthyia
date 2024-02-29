#include "LundAnalysis.h"

int example_C_rhoplus(std::string pattern = "out/tmp3/gen/pythia8/",
                      std::string output_file = "example_C_out.root") {
    
    // Initialize LundAnalysis with the specific filename, output file, and kinematics settings
    LundAnalysis analysis(pattern, output_file, HadroniumAnalysisType::SingleHadron, 1 );
    
    // Set the criteria for selecting rho+ events
    analysis.setCriteria("(211) + (22 22)");
    
    // Set up custom filtering rules for rho+
    FilterRules rules;
    rules.addParticleCondition({213, -1}); // rho+ parentPid and grandParentPid
    rules.addParticleCondition({111, 213}); // parentPid must be 111 for diphoton, with rho+ as grandParentPid
    rules.addParentIdRelationship({0, 1, {RelationshipType::ParentIdAsOtherGrandParentId}});

    // Apply the custom filtering rules
    analysis.setFilterRules(rules);
    
    // Add a z cut
    analysis.addKinematicCut(KinematicCut("z", KinematicCut::CutType::MIN, 0.1)); // z > 0.1

    // Run the analysis
    analysis.run();
    
    return 0;
}