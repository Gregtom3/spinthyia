#include "LUNDAnalysis.h"

int example_C_rhoplus() {
    // Initialize LundAnalysis with the specific filename, output file, and kinematics settings
    LundAnalysis analysis("out/tmp/gen/stringspinner.pythia8.gemc.lund.LU.1.0000.dat", "example_C_out.root", HadroniumAnalysisType::SingleHadron);
    
    // Set the criteria for selecting rho+ events
    analysis.setCriteria("(211) + (22 22)");
    
    // Set up custom filtering rules for rho+
    FilterRules rules;
    rules.addParticleCondition({213, -1}); // rho+ parentPid and grandParentPid
    rules.addParticleCondition({111, 213}); // parentPid must be 111 for diphoton, with rho+ as grandParentPid
    rules.addParentIdRelationship({0, 1, {RelationshipType::ParentIdAsOtherGrandParentId}});
    
    // Apply the custom filtering rules
    analysis.setFilterRules(rules);
    
    // Run the analysis
    analysis.run();
    
    return 0;
}