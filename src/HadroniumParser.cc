#include "HadroniumParser.h"
#include <cmath>
using namespace std;

Hadronium::Hadronium(int pid, int status, double px, double py, double pz, double e, std::vector<int> ids, int parentId, int parentPid, int grandParentId, int grandParentPid)
    : pid(pid), status(status), px(px), py(py), pz(pz), e(e), ids(ids), parentId(parentId), parentPid(parentPid), grandParentId(grandParentId), grandParentPid(grandParentPid) {
        if (ids.empty()) {
            this->ids.push_back(pid);
        }
    }

Hadronium combine_particles(const std::vector<Hadronium>& particles) {
    std::vector<int> combined_ids;
    double total_px = 0, total_py = 0, total_pz = 0, total_e = 0;
    std::set<int> statuses, parentIds, parentPids, grandParentIds, grandParentPids;
    for (const auto& particle : particles) {
        combined_ids.insert(combined_ids.end(), particle.ids.begin(), particle.ids.end());
        total_px += particle.px;
        total_py += particle.py;
        total_pz += particle.pz;
        total_e += particle.e;
        statuses.insert(particle.status);
        parentIds.insert(particle.parentId);
        parentPids.insert(particle.parentPid);
        grandParentIds.insert(particle.grandParentId);
        grandParentPids.insert(particle.grandParentPid);
    }
    int status   = parentIds.size() == 1 ? *statuses.begin() : -1;
    int parentId = parentIds.size() == 1 ? *parentIds.begin() : -1;
    int parentPid = parentIds.size() == 1 ? *parentPids.begin() : -1;
    int grandParentId = grandParentIds.size() == 1 ? *grandParentIds.begin() : -1;
    int grandParentPid = grandParentIds.size() == 1 ? *grandParentPids.begin() : -1;
    
    return Hadronium(0, status, total_px, total_py, total_pz, total_e, combined_ids, parentId, parentPid, grandParentId, grandParentPid);
}

// Finds all particles in an event with a specific PID
std::vector<Hadronium> find_particles(const std::vector<Hadronium>& event, int pid) {
    std::vector<Hadronium> found_particles;
    std::copy_if(event.begin(), event.end(), std::back_inserter(found_particles),
                 [pid](const Hadronium& particle) { return particle.pid == pid; });
    return found_particles;
}

// Helper function to get combinations of particles
template<typename T>
void get_combinations(const std::vector<T>& elements, int count, std::vector<std::vector<T>>& out_combinations) {
    std::vector<bool> v(elements.size());
    std::fill(v.end() - count, v.end(), true);

    do {
        std::vector<T> combination;
        for (int i = 0; i < v.size(); ++i) {
            if (v[i]) {
                combination.push_back(elements[i]);
            }
        }
        out_combinations.push_back(combination);
    } while (std::next_permutation(v.begin(), v.end()));
}

// Reconstructs particles based on a group criterion, creating combinations as needed
std::vector<Hadronium> reconstruct_from_group(const std::vector<Hadronium>& event, const std::string& group) {
    std::istringstream iss(group);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>{}};

    // Count the occurrences of each PID in the group
    std::map<int, int> pid_counts;
    for (const auto& token : tokens) {
        int pid = std::stoi(token);
        pid_counts[pid]++;
    }

    std::vector<Hadronium> reconstructed;
    for (const auto& [pid, count] : pid_counts) {
        auto particles = find_particles(event, pid);
        if (particles.size() < count){
            return std::vector<Hadronium>(); // return empty vector if not enough particles of the desired pid are found
        }
        else if (count == 1) {
            reconstructed.insert(reconstructed.end(), particles.begin(), particles.end());
        } else {
            // Generate and combine all unique combinations for multiple occurrences
            std::vector<std::vector<Hadronium>> combinations;
            get_combinations(particles, count, combinations);
            for (const auto& combo : combinations) {
                reconstructed.push_back(combine_particles(combo));
            }
        }
    }

    return reconstructed;
}


std::vector<std::vector<Hadronium>> combine_lists(const std::vector<std::vector<Hadronium>>& x, const std::vector<std::vector<Hadronium>>& y) {
    std::vector<std::vector<Hadronium>> result;
    for (const auto& a : x) {
        for (const auto& b : y) {
            std::vector<Hadronium> combined = a;
            combined.insert(combined.end(), b.begin(), b.end());
            result.push_back(combined);
        }
    }
    return result;
}

bool has_shared_ids(const std::vector<Hadronium>& combination) {
    std::set<int> all_ids;
    for (const auto& particle : combination) {
        all_ids.insert(particle.ids.begin(), particle.ids.end());
    }
    int total_ids = 0;
    for (const auto& particle : combination) {
        total_ids += particle.ids.size();
    }
    return total_ids != all_ids.size();
}

std::vector<std::vector<Hadronium>> filter_duplicate_combinations(const std::vector<std::vector<Hadronium>>& combinations) {
    std::vector<std::vector<Hadronium>> unique_combinations;
    std::set<std::set<int>> seen;
    for (const auto& combination : combinations) {
        std::set<int> sorted_ids;
        for (const auto& particle : combination) {
            sorted_ids.insert(particle.ids.begin(), particle.ids.end());
        }
        if (seen.find(sorted_ids) == seen.end()) {
            unique_combinations.push_back(combination);
            seen.insert(sorted_ids);
        }
    }
    return unique_combinations;
}

std::vector<std::vector<Hadronium>> reconstruct_hadronia(LundEvent& event, const std::string& criteria, AcceptanceType acc) {
    auto hadronia = convertLundEventToHadronia(event, acc);
    std::regex pattern("\\(([^()]+)\\)");
    std::vector<std::vector<Hadronium>> reconstructed;
    auto begin = std::sregex_iterator(criteria.begin(), criteria.end(), pattern);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string group = match.str(1);
        auto group_particles = reconstruct_from_group(hadronia, group);
        if (group_particles.size()==0){
            return std::vector<std::vector<Hadronium>>(); // return empty vector if not enough particles are found
        }
        else if (reconstructed.empty()) {
            for (const auto& particle : group_particles) {
                reconstructed.push_back({particle});
            }
        } else {
            std::vector<std::vector<Hadronium>> temp;
            for (const auto& particle : group_particles) {
                temp.push_back({particle});
            }
            auto new_combinations = combine_lists(reconstructed, temp);
            reconstructed = new_combinations;
            reconstructed.erase(std::remove_if(reconstructed.begin(), reconstructed.end(), has_shared_ids), reconstructed.end());
        }
    }
    return filter_duplicate_combinations(reconstructed);
}


std::vector<Hadronium> convertLundEventToHadronia(LundEvent& event, AcceptanceType acc) {
    std::vector<Hadronium> hadronia;
    std::vector<LundParticle> lundParticles = event.particles;
    for (const auto& lundParticle : lundParticles) {
        // Extract relevant information from LundParticle
        int pid = lundParticle.particle_id;
        int status = lundParticle.status;
        if (status != 1) continue; // Ignore non-final state particles
        float lifetime = lundParticle.lifetime;
        if (lifetime == -1.0) continue; // Ignore final state particles with diquark ancestor
        double px = lundParticle.px;
        double py = lundParticle.py;
        double pz = lundParticle.pz;
        double p = sqrt(px*px + py*py + pz*pz);
        double e = lundParticle.e;
        double theta = 180/3.14159265*acos(pz /  p );
        if (acc == AcceptanceType::CLAS12){ // Acceptance of the CLAS12 Spectrometer
            if (theta < 5 || theta > 35) continue;
            if (abs(pid)==211 && p < 1.25) continue;
            if (pid==22 && e < 0.2) continue;
        }
        double m = lundParticle.m;
        int parentId = lundParticle.index_of_parent;
        int parentPid = 0;
        int grandParentId = 0;
        int grandParentPid = 0;
        if ( parentId > 0){
            grandParentId = lundParticles.at(parentId-1).index_of_parent;
            parentPid = lundParticles.at(parentId-1).particle_id;
            if (grandParentId > 0){
                grandParentPid = lundParticles.at(grandParentId-1).particle_id;
            }
        }
        std::vector<int> ids = {lundParticle.index};

        // Create a Hadronium object with the extracted information
        Hadronium h(pid, status, px, py, pz, e, ids, parentId, parentPid, grandParentId, grandParentPid);

        // Add the Hadronium object to the vector
        hadronia.push_back(h);
    }
    return hadronia;
}


// Function to print hadroniums
void printHadronia(const std::vector<std::vector<Hadronium>>& hadroniums) {
    std::cout << "-------------------*** Printing Event ***-------------------" << std::endl;
    int nH = 0;
    for (const auto& hadronium : hadroniums) {
        std::cout << "\t Hadronia #" << nH++ << ":" << std::endl;
        for (const auto& p : hadronium) {
            std::cout << "\t\tIDs ";
            for (int id : p.ids) {
                std::cout << id << " ";
            }
            std::cout << "| Px: " << p.px << " Py: " << p.py << " Pz: " << p.pz << " E: " << p.e 
                 << " M: " << sqrt(std::max(p.e*p.e - p.px*p.px - p.py*p.py - p.pz*p.pz, 0.0))
                 << " | ParentId: " << p.parentId << " ParentPid: " << p.parentPid 
                 << " | GrandParentId: " << p.grandParentId << " GrandParentPid: " << p.grandParentPid<< std::endl;
        }
    }
}

// ------------------- HADRONIA FILTERING CODE -------------------------------------------
// Structure to hold individual particle conditions
struct ParticleCondition {
    int requiredParentPid = -1; // -1 means any parentPid is acceptable
    int requiredGrandParentPid = -1; // -1 means any grandParentPid is acceptable
};

// Structure to define relationships between particles within a hadronium
struct ParentIdRelationship {
    size_t particleIndex1;
    size_t particleIndex2;
    bool enforceSameParentId = false; // True to enforce same parentId for particleIndex1 and particleIndex2
};

// Class to encapsulate filtering rules
class FilterRules {
public:
    std::vector<ParticleCondition> particleConditions;
    std::vector<ParentIdRelationship> parentIdRelationships;

    void addParticleCondition(const ParticleCondition& condition) {
        particleConditions.push_back(condition);
    }

    void addParentIdRelationship(const ParentIdRelationship& relationship) {
        parentIdRelationships.push_back(relationship);
    }
};

