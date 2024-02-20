#include "Pythia8/Pythia.h"
#include "StringSpinner.h"
#include "TLorentzVector.h"
#include "TVector3.h"

#include <fstream>
#include <iomanip> 

using namespace Pythia8;

double get_mass(Vec4 vec){
    return sqrt(pow(vec.e(),2)-pow(vec.px(),2)-pow(vec.py(),2)-pow(vec.pz(),2));
}

TLorentzVector get_tlorentzvector(Event& event, int index){
    Vec4 vec = event[index].p();
    TLorentzVector vec2;
    vec2.SetPxPyPzE(vec.px(),vec.py(),vec.pz(),vec.e());
    return vec2;
}

int main(int argc, char* argv[]) {
  if (argc < 6) {
    std::cout << "Usage: " << argv[0] << " <path/to/output> <path/to/runcard.cmnd> <nEvent> <mode> <seed>" << std::endl;
    return 1;
  }
  std::string outputFilePath = argv[1];
  std::string runCardName    = argv[2];
  int nEvent = std::atoi(argv[3]);
  int mode   = std::atoi(argv[4]);
  int seed   = std::atoi(argv[5]);
    
    
  const std::string baseFilePrefix = "stringspinner.pythia8.gemc.lund."; // File prefix
  std::string filePrefix;
   
  switch(mode){
      case 0:
          filePrefix = baseFilePrefix+"LU.1.";
          break;
      case 1:
          filePrefix = baseFilePrefix+"LU.-1.";
          break;
      case 2:
          filePrefix = baseFilePrefix+"UL.1.";
          break;
      case 3:
          filePrefix = baseFilePrefix+"UL.-1.";
          break;
      default:
          std::cerr << "Invalid mode value. Must be 0,1,2, or 3" << std::endl;
          return -1;
  }
    
  int fileIndex = 0; // Index for file naming
  int eventsPerFile = 10000; // Number of events per file
    
    
  Pythia pythia;
  Event& event = pythia.event;
  auto fhooks = std::make_shared<SimpleStringSpinner>();
  fhooks->plugInto(pythia); 

  // load steering file
  pythia.readFile(runCardName);
    
    
  // Seed
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = " + std::to_string(seed));
    
  // Choose to assign polarisations.
  int beamSpin, targetSpin;
  bool beamPolarized   = false;
  bool targetPolarized = false;
  switch(mode) {
    case 0: // LU, spin +
      beamPolarized = true;
      beamSpin      = 1;
      targetSpin    = 0;
      break;
    case 1: // LU, spin -
      beamPolarized = true;
      beamSpin      = -1;
      targetSpin    = 0;
      break;
    case 2: // UL, spin +
      targetPolarized = true;
      beamSpin        = 0;
      targetSpin      = 1;
      break;
    case 3: // UL, spin -
      targetPolarized = true;
      beamSpin        = 0;
      targetSpin      = -1;
      break;
  }
    
  
  std::string polStr;
  Vec4 SNucleon;
  if(beamPolarized) {
    switch(beamSpin) {
      case 1:  polStr = "0.0,0.0,-1.0"; break; // minus sign, since quark momentum is reversed after hard scattering
      case -1: polStr = "0.0,0.0,1.0";  break; // minus sign, since quark momentum is reversed after hard scattering
    }
    std::vector<std::string> quarks = {"u", "d", "s", "ubar", "dbar", "sbar"};
    for(auto quark : quarks)
      pythia.readString("StringSpinner:" + quark + "Polarisation = " + polStr);
  }
  if(targetPolarized) {
    SNucleon.p(0.0, 0.0, (double)targetSpin, 0.0);
    switch(targetSpin) {
      case 1:  polStr = "0.0,0.0,1.0";  break;
      case -1: polStr = "0.0,0.0,-1.0"; break;
    }
    pythia.readString("StringSpinner:targetPolarisation = " + polStr);
  }
  
    
   
  // Initialize.
  pythia.init();

    
  // Header variables for LUND
  int nParticles;
  float mass_target;
  int atomic_number_target = 1; // assuming proton
  int target_polarization = targetSpin;
  int beam_polarization = beamSpin;
  int beam_type;
  float beam_energy;
  int interacted_nucleon_id;
  int process_id;
  float event_weight;
    
  // Particle variables for LUND GEMC
  int index;
  float lifetime = 0.0;
  int status;
  int particle_id;
  int index_of_parent;
  int index_of_first_daughter;
  float px;
  float py;
  float pz;
  float e;
  float m;
  float vx;
  float vy;
  float vz;
    
  const double eps = 1e-9; // Threshold for considering a value as zero
  // Begin event loop.
  std::ofstream outFile;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    // Open a new file at the start or every eventsPerFile events
    if (iEvent % eventsPerFile == 0) {
        if (outFile.is_open()) {
            outFile.close(); // Close the current file if it's open
        }
        std::stringstream fileName;
        fileName << outputFilePath << "/" << filePrefix
                 << std::setw(4) << std::setfill('0') << fileIndex
                 << ".dat";
        outFile.open(fileName.str());
        if (!outFile.is_open()) {
            std::cerr << "Failed to open file: " << fileName.str() << std::endl;
            return -1; // or handle error appropriately
        }
        fileIndex++; // Increment fileIndex for the next file
    }

    if (!pythia.next()) continue;

    if (iEvent < 20){event.list();}
   
    TLorentzVector init_lepton = get_tlorentzvector(event,1);
    TLorentzVector init_target = get_tlorentzvector(event,2);
    TLorentzVector scattered_lepton = get_tlorentzvector(event,5);
    TLorentzVector q = init_lepton - scattered_lepton;
    
    nParticles = event.size();
    mass_target = init_target.M();
    beam_type  = event[1].id();
    beam_energy = init_lepton.E();
    interacted_nucleon_id = event[2].id();
    process_id = pythia.info.code();
    event_weight = pythia.info.weight();
      
    // Assuming outFile is an std::ofstream and is already open
outFile << "\t" << std::left << std::setw(8) << nParticles << std::setw(8) << mass_target << std::setw(8) << atomic_number_target << std::setw(8) << target_polarization << std::setw(8) << beam_polarization << std::setw(8) << beam_type << std::setw(8) << beam_energy << std::setw(8) << interacted_nucleon_id << std::setw(8) << process_id << std::setw(8) << event_weight << "\n";

    // Loop inside the event output.
    for (int i = 0; i < event.size(); ++i){
        TLorentzVector particle = get_tlorentzvector(event,i);
        index = i;
        status      = event[i].isFinal(); // 1 --> propogate through GEANT
        particle_id = event[i].id();
        index_of_parent = event[i].mother1();
        index_of_first_daughter = event[i].daughter1();
        px = particle.Px();
        py = particle.Py();
        pz = particle.Pz();
        e  = particle.E();
        m  = particle.M();
        vx = event[i].xProd()/10.; // mm -> cm
        vy = event[i].yProd()/10.; // mm -> cm
        vz = event[i].zProd()/10.; // mm -> cm
        
        // Apply threshold check and adjust values
        if (std::abs(px) < eps) px = 0.0;
        if (std::abs(py) < eps) py = 0.0;
        if (std::abs(pz) < eps) pz = 0.0;
        if (std::abs(vx) < eps) vx = 0.0;
        if (std::abs(vy) < eps) vy = 0.0;
        if (std::abs(vz) < eps) vz = 0.0;
        
        // Output using fixed and setprecision
        outFile << std::left << std::setw(4) << index 
                << std::setw(8) << std::setprecision(5) << std::fixed << lifetime 
                << std::setw(4) << status 
                << std::setw(8) << particle_id 
                << std::setw(8) << index_of_parent 
                << std::setw(8) << index_of_first_daughter 
                << std::setw(12) << std::setprecision(6) << std::fixed << px 
                << std::setw(12) << std::setprecision(6) << std::fixed << py 
                << std::setw(12) << std::setprecision(6) << std::fixed << pz 
                << std::setw(12) << std::setprecision(6) << std::fixed << e 
                << std::setw(12) << std::setprecision(6) << std::fixed << m 
                << std::setw(12) << std::setprecision(6) << std::fixed << vx 
                << std::setw(12) << std::setprecision(6) << std::fixed << vy 
                << std::setw(12) << std::setprecision(6) << std::fixed << vz << "\n";
    }
  }
    
  if (outFile.is_open()) {
    outFile.close(); // Make sure to close the last file
  }
    
  return 0;
}