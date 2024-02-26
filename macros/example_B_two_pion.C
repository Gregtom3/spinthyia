#include "LundReader.h"
#include "HadroniumParser.h"
#include "HadroniaFilter.h"
#include "Kinematics.h"
#include "DISTree.h"


R__LOAD_LIBRARY(Spinthyia)
    
int example_B_two_pion(
    const std::string filename = "out/tmp/gen/stringspinner.pythia8.gemc.lund.LU.1.0000.dat",
    string criteria = "(211) + (-211)"
){
    
    // Setup output TFile
    DISTree distree = DISTree("example_B_out.root", false, true); // Don't do single hadron kinematics, do dihadron kinematics
    
    // Load the LundReader
    LundReader reader(filename);
    
    // Loop over LUND events
    LundEvent event;
    int iEvent = 0;
    while (reader.readEvent(event)) {
        
        // Get the hadronia based on the user inputted criteria
        std::vector<vector<Hadronium>> hadronia = reconstruct_hadronia(event, criteria);
        
        
        // If the event contains nothing we want, continue
        if (hadronia.size() == 0) continue;

        // Fill the TTree
        distree.Fill(event, hadronia);
            
        // Print out a few events
        if(iEvent < 20) printHadronia(hadronia);
        iEvent++;
    }   
    
    distree.Write();
    
    return 0;
}