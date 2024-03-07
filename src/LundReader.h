#ifndef LUND_READER_H
#define LUND_READER_H

#include <fstream>
#include <string>
#include <vector>
#include <sstream>

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
    double mass_target;
    int atomic_number_target;
    int target_polarization;
    int beam_polarization;
    int beam_type;
    double beam_energy;
    int interacted_nucleon_id;
    int process_id;
    double event_weight;
    std::vector<LundParticle> particles;
};

// Class to read Lund data from a .dat file
class LundReader {
private:
    std::ifstream inFile;
    std::string filename;

public:
    LundReader(const std::string& fname);
    ~LundReader();
    bool readEvent(LundEvent& event);
};

void analyzeEvent(const LundEvent& event);

#endif // LUND_READER_H