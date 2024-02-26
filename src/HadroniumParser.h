#ifndef HADRONIUM_PARSER_H
#define HADRONIUM_PARSER_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include <iterator>
#include <algorithm>

class Particle {
public:
    int pid; // Particle ID
    double px, py, pz, e, m; // Physical properties
    std::vector<int> ids; // IDs for composite particles
    int parentId; // ID of the parent particle
    int parentPid; // PID of the parent particle

    Particle(int pid, double px, double py, double pz, double e, std::vector<int> ids = {}, int parentId = -1, int parentPid = -1);
};

Particle combine_particles(const std::vector<Particle>& particles);
std::vector<Particle> find_particles(const std::vector<Particle>& event, int pid);

template<typename T>
void get_combinations(const std::vector<T>& elements, int count, std::vector<std::vector<T>>& out_combinations);

std::vector<Particle> reconstruct_from_group(const std::vector<Particle>& event, const std::string& group);
std::vector<std::vector<Particle>> combine_lists(const std::vector<std::vector<Particle>>& x, const std::vector<std::vector<Particle>>& y);
bool has_shared_ids(const std::vector<Particle>& combination);
std::vector<std::vector<Particle>> filter_duplicate_combinations(const std::vector<std::vector<Particle>>& combinations);
std::vector<std::vector<Particle>> reconstruct_multi_particle(const std::vector<Particle>& event, const std::string& criteria);

#endif // HADRONIUM_PARSER_H

