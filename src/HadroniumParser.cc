#include "HadroniumParser.h"

Particle::Particle(int pid, double px, double py, double pz, double e, std::vector<int> ids, int parentId, int parentPid)
    : pid(pid), px(px), py(py), pz(pz), e(e), ids(ids), parentId(parentId), parentPid(parentPid) {
        if (ids.empty()) {
            this->ids.push_back(pid);
        }
    }

Particle combine_particles(const std::vector<Particle>& particles) {
    std::vector<int> combined_ids;
    double total_px = 0, total_py = 0, total_pz = 0, total_e = 0;
    std::set<int> parentIds, parentPids;

    for (const auto& particle : particles) {
        combined_ids.insert(combined_ids.end(), particle.ids.begin(), particle.ids.end());
        total_px += particle.px;
        total_py += particle.py;
        total_pz += particle.pz;
        total_e += particle.e;
        parentIds.insert(particle.parentId);
        parentPids.insert(particle.parentPid);
    }
    
    int parentId = parentIds.size() == 1 ? *parentIds.begin() : -1;
    int parentPid = parentIds.size() == 1 ? *parentPids.begin() : -1;

    return Particle(0, total_px, total_py, total_pz, total_e, combined_ids, parentId, parentPid);
}

// Finds all particles in an event with a specific PID
std::vector<Particle> find_particles(const std::vector<Particle>& event, int pid) {
    std::vector<Particle> found_particles;
    std::copy_if(event.begin(), event.end(), std::back_inserter(found_particles),
                 [pid](const Particle& particle) { return particle.pid == pid; });
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
std::vector<Particle> reconstruct_from_group(const std::vector<Particle>& event, const std::string& group) {
    std::istringstream iss(group);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>{}};

    // Count the occurrences of each PID in the group
    std::map<int, int> pid_counts;
    for (const auto& token : tokens) {
        int pid = std::stoi(token);
        pid_counts[pid]++;
    }

    std::vector<Particle> reconstructed;
    for (const auto& [pid, count] : pid_counts) {
        auto particles = find_particles(event, pid);
        if (particles.size() < count){
            return std::vector<Particle>(); // return empty vector if not enough particles of the desired pid are found
        }
        else if (count == 1) {
            reconstructed.insert(reconstructed.end(), particles.begin(), particles.end());
        } else {
            // Generate and combine all unique combinations for multiple occurrences
            std::vector<std::vector<Particle>> combinations;
            get_combinations(particles, count, combinations);
            for (const auto& combo : combinations) {
                reconstructed.push_back(combine_particles(combo));
            }
        }
    }

    return reconstructed;
}


std::vector<std::vector<Particle>> combine_lists(const std::vector<std::vector<Particle>>& x, const std::vector<std::vector<Particle>>& y) {
    std::vector<std::vector<Particle>> result;
    for (const auto& a : x) {
        for (const auto& b : y) {
            std::vector<Particle> combined = a;
            combined.insert(combined.end(), b.begin(), b.end());
            result.push_back(combined);
        }
    }
    return result;
}

bool has_shared_ids(const std::vector<Particle>& combination) {
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

std::vector<std::vector<Particle>> filter_duplicate_combinations(const std::vector<std::vector<Particle>>& combinations) {
    std::vector<std::vector<Particle>> unique_combinations;
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

std::vector<std::vector<Particle>> reconstruct_multi_particle(const std::vector<Particle>& event, const std::string& criteria) {
    std::regex pattern("\\(([^()]+)\\)");
    std::vector<std::vector<Particle>> reconstructed;
    auto begin = std::sregex_iterator(criteria.begin(), criteria.end(), pattern);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string group = match.str(1);
        auto group_particles = reconstruct_from_group(event, group);
        if (group_particles.size()==0){
            return std::vector<std::vector<Particle>>(); // return empty vector if not enough particles are found
        }
        else if (reconstructed.empty()) {
            for (const auto& particle : group_particles) {
                reconstructed.push_back({particle});
            }
        } else {
            std::vector<std::vector<Particle>> temp;
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