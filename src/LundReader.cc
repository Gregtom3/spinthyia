#include "LundReader.h"

LundReader::LundReader(const std::string& fname) : filename(fname) {
  // Check if the file has a .root extension
  if (filename.substr(filename.find_last_of(".") + 1) == "root"){
      if (fIn!=0){
        if(fIn->IsOpen()){
          fIn->Close();
        }
      }
      // Load TFile and TTree
      fIn = new TFile(filename.c_str(), "READ");
      if (!fIn || !fIn->IsOpen()) {
          throw std::runtime_error("Unable to open ROOT file: " + filename);
      }
      tIn = (TTree*)(fIn->Get("tree"));
      if (!tIn) {
          throw std::runtime_error("Unable to find TTree named 'tree' in file: " + filename);
      }
      tIn->SetBranchAddress("nParticles", &levent.nParticles);
      tIn->SetBranchAddress("mass_target", &levent.mass_target);
      tIn->SetBranchAddress("atomic_number_target", &levent.atomic_number_target);
      tIn->SetBranchAddress("target_polarization", &levent.target_polarization);
      tIn->SetBranchAddress("beam_polarization", &levent.beam_polarization);
      tIn->SetBranchAddress("beam_type", &levent.beam_type);
      tIn->SetBranchAddress("beam_energy", &levent.beam_energy);
      tIn->SetBranchAddress("interacted_nucleon_id", &levent.interacted_nucleon_id);
      tIn->SetBranchAddress("process_id", &levent.process_id);
      tIn->SetBranchAddress("event_weight", &levent.event_weight);
    
      tIn->SetBranchAddress("index", &index);
      tIn->SetBranchAddress("lifetime", &lifetime);
      tIn->SetBranchAddress("status", &status);
      tIn->SetBranchAddress("particle_id", &particle_id);
      tIn->SetBranchAddress("index_of_parent", &index_of_parent);
      tIn->SetBranchAddress("index_of_first_daughter", &index_of_first_daughter);
      tIn->SetBranchAddress("px", &px);
      tIn->SetBranchAddress("py", &py);
      tIn->SetBranchAddress("pz", &pz);
      tIn->SetBranchAddress("e", &e);
      tIn->SetBranchAddress("m", &m);
      tIn->SetBranchAddress("vx", &vx);
      tIn->SetBranchAddress("vy", &vy);
      tIn->SetBranchAddress("vz", &vz);
      isTFile = true;
      isDat   = false;
  } else {
      inFile.open(filename);
      if (!inFile.is_open()) {
          throw std::runtime_error("Unable to open file: " + filename);
      }
      isTFile = false;
      isDat = true;
  }
}

LundReader::~LundReader() {
    if (inFile.is_open()) {
        inFile.close();
    }
}

bool LundReader::readEvent(LundEvent& event) {
    eventCount++;
    event.particles.clear(); // Clear any existing particles
    if (isDat == true){
      std::string line;

      // Read the header line for the event
      if (!std::getline(inFile, line)) return false;
      std::istringstream headerStream(line);
      headerStream >> event.nParticles >> event.mass_target >> event.atomic_number_target
                   >> event.target_polarization >> event.beam_polarization >> event.beam_type
                   >> event.beam_energy >> event.interacted_nucleon_id >> event.process_id
                   >> event.event_weight;

      // Read Lund particle information for the event
      for (int i = 0; i < event.nParticles; ++i) {
          if (!std::getline(inFile, line)) return false;
          std::istringstream particleStream(line);
          LundParticle particle;
          particleStream >> particle.index >> particle.lifetime >> particle.status >> particle.particle_id
                         >> particle.index_of_parent >> particle.index_of_first_daughter >> particle.px
                         >> particle.py >> particle.pz >> particle.e >> particle.m >> particle.vx
                         >> particle.vy >> particle.vz;
          event.particles.push_back(particle);
      }
    }
    else if (isTFile == true){
      if(eventCount==tIn->GetEntries()) return false;
      tIn->GetEntry(eventCount);
      event.nParticles = levent.nParticles;
      event.mass_target = levent.mass_target;
      event.atomic_number_target = levent.atomic_number_target;
      event.target_polarization = levent.target_polarization;
      event.beam_polarization = levent.beam_polarization;
      event.beam_type = levent.beam_type;
      event.beam_energy = levent.beam_energy;
      event.interacted_nucleon_id = levent.interacted_nucleon_id;
      event.process_id = levent.process_id;
      event.event_weight = levent.event_weight;
      for (int i = 0; i < px->size(); i++) {
          LundParticle particle{index->at(i), lifetime->at(i), status->at(i), particle_id->at(i), 
                                index_of_parent->at(i), index_of_first_daughter->at(i), 
                                px->at(i), py->at(i), pz->at(i), 
                                e->at(i), m->at(i), vx->at(i), vy->at(i), vz->at(i)}; 
          event.particles.push_back(particle);
      }
    }
    return true;
}
