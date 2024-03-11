#include "Pythia8/Pythia.h"
#include "StringSpinner.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

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

// Function to check if any ancestor particle was a diquark
bool hasDiquarkAncestor(Event& event, int currentParticleIndex) {
    int parentIndex = event[currentParticleIndex].mother1();
    // Base case: no parent
    if (parentIndex <= 0) {
        return false;
    }
    
    if (event[parentIndex].isDiquark()) {
        return true;
    }
    // Recursive call to check the next ancestor in the lineage
    return hasDiquarkAncestor(event, parentIndex);
}

int main(int argc, char* argv[]) {
  if (argc < 6) {
    std::cout << "Usage: " << argv[0] << " <path/to/output> <path/to/runcard.cmnd> <nEvent> <mode> <seed> <optional: batch>" << std::endl;
    return 1;
  }
  std::string outputFilePath = argv[1];
  std::string runCardName    = argv[2];
  int nEvent = std::atoi(argv[3]);
  int mode   = std::atoi(argv[4]);
  int seed   = std::atoi(argv[5]);
  int batch  = -1;
  std::string baseFilePrefixPrefix="";
  if (argc == 7){
      batch = std::atoi(argv[6]);
      baseFilePrefixPrefix=Form("batch%d_",batch);
  }
    
  const std::string baseFilePrefix = baseFilePrefixPrefix+"stringspinner.pythia8.gemc.lund."; // File prefix
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
  int eventsPerFile = 100000; // Number of events per file
    
    
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
      case -1: polStr = "0.0,0.0,1.0";  break; 
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
  std::vector<int> index;
  std::vector<float> lifetime;
  std::vector<int> status;
  std::vector<int> particle_id;
  std::vector<int> index_of_parent;
  std::vector<int> index_of_first_daughter;
  std::vector<int> index_of_grandparent;
  std::vector<float> px;
  std::vector<float> py;
  std::vector<float> pz;
  std::vector<float> e;
  std::vector<float> m;
  std::vector<float> vx;
  std::vector<float> vy;
  std::vector<float> vz;

  // Info for the current particle
  
  int _index;
  float _lifetime;
  int _status;
  int _particle_id;
  int _index_of_parent;
  int _index_of_first_daughter;
  int _index_of_grandparent;
  float _px;
  float _py;
  float _pz;
  float _e;
  float _m;
  float _vx;
  float _vy;
  float _vz;
  
  std::string fileName = outputFilePath + "/" + filePrefix + "0000.root";
  TFile * fOut = new TFile(fileName.c_str(),"RECREATE");
  TTree *tree  = new TTree("tree","tree");
  // Header variables for LUND
  tree->Branch("nParticles", &nParticles);
  tree->Branch("mass_target", &mass_target);
  tree->Branch("atomic_number_target", &atomic_number_target);
  tree->Branch("target_polarization", &target_polarization);
  tree->Branch("beam_polarization", &beam_polarization);
  tree->Branch("beam_type", &beam_type);
  tree->Branch("beam_energy", &beam_energy);
  tree->Branch("interacted_nucleon_id", &interacted_nucleon_id);
  tree->Branch("process_id", &process_id);
  tree->Branch("event_weight", &event_weight);

  // Particle variables for LUND GEMC
  tree->Branch("index", &index);
  tree->Branch("lifetime", &lifetime);
  tree->Branch("status", &status);
  tree->Branch("particle_id", &particle_id);
  tree->Branch("index_of_parent", &index_of_parent);
  tree->Branch("index_of_first_daughter", &index_of_first_daughter);
  tree->Branch("index_of_grandparent", &index_of_grandparent);
  tree->Branch("px", &px);
  tree->Branch("py", &py);
  tree->Branch("pz", &pz);
  tree->Branch("e", &e);
  tree->Branch("m", &m);
  tree->Branch("vx", &vx);
  tree->Branch("vy", &vy);
  tree->Branch("vz", &vz);
  
  const double eps = 1e-9; // Threshold for considering a value as zero
  
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    
    index.clear();
    lifetime.clear();
    status.clear();
    particle_id.clear();
    index_of_parent.clear();
    index_of_first_daughter.clear();
    index_of_grandparent.clear();
    px.clear();
    py.clear();
    pz.clear();
    e.clear();
    m.clear();
    vx.clear();
    vy.clear();
    vz.clear();

    if (!pythia.next()) continue;

    if (iEvent < 20){event.list();}
   
    TLorentzVector init_lepton = get_tlorentzvector(event,1);
    TLorentzVector init_target = get_tlorentzvector(event,2);
    TLorentzVector scattered_lepton = get_tlorentzvector(event,5);
    TLorentzVector q = init_lepton - scattered_lepton;
    
    nParticles = event.size() - 1; // -1 --> accounts for PID==90, which represents the "system"
    mass_target = init_target.M();
    beam_type  = event[1].id();
    beam_energy = init_lepton.E();
    interacted_nucleon_id = event[2].id();
    process_id = pythia.info.code();
    event_weight = pythia.info.weight();
      

    // Loop inside the event output.
    for (int i = 0; i < event.size(); ++i){
        TLorentzVector particle = get_tlorentzvector(event,i);
        _index = i;
        _status      = event[i].isFinal(); // 1 --> propogate through GEANT
        _particle_id = event[i].id();
        if ( _particle_id == 90 ){ continue; } // skip PID==90 (system)
        _index_of_parent = event[i].mother1();
        
        // Use the recursive function to check for an ancestor diquark
        if (hasDiquarkAncestor(event, i)) {
            _lifetime = -1.0; // If true, set lifetime to -1
        }else{
            _lifetime = 1.0;
        }
        
        _index_of_first_daughter = event[i].daughter1();
        _px = particle.Px();
        _py = particle.Py();
        _pz = particle.Pz();
        _e  = particle.E();
        _m  = particle.M();
        _vx = event[i].xProd()/10.; // mm -> cm
        _vy = event[i].yProd()/10.; // mm -> cm
        _vz = event[i].zProd()/10.; // mm -> cm
        
        index.push_back(_index);
        lifetime.push_back(_lifetime);
        status.push_back(_status);
        particle_id.push_back(_particle_id);
        index_of_parent.push_back(_index_of_parent);
        index_of_first_daughter.push_back(_index_of_first_daughter);
        px.push_back(_px);
        py.push_back(_py);
        pz.push_back(_pz);
        e.push_back(_e);
        m.push_back(_m);
        vx.push_back(_vx);
        vy.push_back(_vy);
        vz.push_back(_vz);
        
    }
    
    tree->Fill();
  }
  
  fOut->cd();
  tree->Write();
  fOut->Close();
    
  return 0;
}