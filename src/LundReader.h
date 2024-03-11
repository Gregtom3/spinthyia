#ifndef LUND_READER_H
#define LUND_READER_H

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "TFile.h"
#include "TTree.h"
#include <iostream>

enum class AcceptanceType {
    ALL,
    CLAS12
};
// Struct to store Lund particle information
struct LundParticle {
    int index;
    double lifetime;
    int status;
    int particle_id;
    int index_of_parent;
    int index_of_first_daughter;
    double px, py, pz, e, m;
    double vx, vy, vz;
};

// Struct to store Lund event information including the header and particles
struct LundEvent {
    int nParticles;
    float mass_target;
    int atomic_number_target;
    int target_polarization;
    int beam_polarization;
    int beam_type;
    float beam_energy;
    int interacted_nucleon_id;
    int process_id;
    float event_weight;
    std::vector<LundParticle> particles;
};

// Class to read Lund data from a .dat file
class LundReader {
private:
    bool isTFile = false;
    bool isDat   = false;
    std::ifstream inFile;
    std::string filename;
    TFile * fIn = 0;
    TTree * tIn = 0;
    LundEvent levent;
    std::vector<int> * index= 0;
    std::vector<float> * lifetime= 0;
    std::vector<int> * status= 0;
    std::vector<int> * particle_id= 0;
    std::vector<int> * index_of_parent= 0;
    std::vector<int> * index_of_first_daughter= 0;
    std::vector<float> * px= 0;
    std::vector<float> * py= 0;
    std::vector<float> * pz= 0;
    std::vector<float> * e= 0;
    std::vector<float> * m= 0;
    std::vector<float> * vx= 0;
    std::vector<float> * vy= 0;
    std::vector<float> * vz= 0;
    int eventCount = -1;
public:
    LundReader(const std::string& fname);
    ~LundReader();
    bool readEvent(LundEvent& event);
};

void analyzeEvent(const LundEvent& event);

#endif // LUND_READER_H