#include "LundReader.h"

LundReader::LundReader(const std::string& fname) : filename(fname) {
    inFile.open(filename);
    if (!inFile.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
}

LundReader::~LundReader() {
    if (inFile.is_open()) {
        inFile.close();
    }
}

bool LundReader::readEvent(LundEvent& event) {
    event.particles.clear(); // Clear any existing particles
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
    return true;
}
