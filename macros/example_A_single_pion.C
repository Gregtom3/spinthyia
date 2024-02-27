#include "LundAnalysis.h"

int example_A_single_pion() {
    LundAnalysis analysis("out/tmp/gen/stringspinner.pythia8.gemc.lund.LU.1.0000.dat", "example_A_out.root", HadroniumAnalysisType::SingleHadron);
    analysis.setCriteria("(211)");
    analysis.run();
    return 0;
}