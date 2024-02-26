#include "LundReader.h"
#include "HadroniumParser.h"
#include "HadroniaFilter.h"
#include "Kinematics.h"
#include "DISTree.h"

R__LOAD_LIBRARY(Spinthyia)
    
int example_C_rhoplus(
    const std::string filename = "out/tmp/gen/stringspinner.pythia8.gemc.lund.LU.1.0000.dat",
    string criteria = "(211) + (22 22)"
){
    
    // Setup output TFile
    DISTree distree = DISTree("example_C_out.root", true, false); // Do single hadron kinematics, don't do dihadron kinematics
    
    // Load the LundReader
    LundReader reader(filename);
    
    // Custom filtering rules to get desired final state
    FilterRules rules;
    
    // Conditions for h_1 --> Piplus
    rules.addParticleCondition({213, -1}); // h_1: rho+ parentPid and grandParentPid
    
    // Conditions for h_2 --> Diphoton
    rules.addParticleCondition({111, 213}); // h_2: parentPid must be 111, rho+ grandParentPid
    
    // Enforce that h_1 has the same parent_ID as h_2
    rules.addParentIdRelationship({0, 1, {RelationshipType::ParentIdAsOtherGrandParentId}});
    
    // Loop over LUND events
    LundEvent event;
    int iEvent = 0;
    while (reader.readEvent(event)) {
        
        // Get the hadronia based on the user inputted criteria
        std::vector<vector<Hadronium>> hadronia = reconstruct_hadronia(event, criteria);
        
        // Filter out based on parental/pid rules
        std::vector<std::vector<Hadronium>> filteredHadronia = filterHadronia(hadronia, rules);
        
        // If the event contains nothing we want, continue
        if (filteredHadronia.size() == 0) continue;

        // Fill the TTree
        distree.Fill(event, filteredHadronia);
            
        // Print out a few events
        if(iEvent < 20) printHadronia(filteredHadronia);
        iEvent++;
    }   
    
    distree.Write();
    return 0;
}