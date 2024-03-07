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
#include "LundReader.h"

class Hadronium {
public:
    int pid; // Particle ID
    int status; // Particle status
    double px, py, pz, e, m; // Physical properties
    std::vector<int> ids; // IDs for composite particles
    int parentId; // ID of the parent particle
    int parentPid; // PID of the parent particle
    int grandParentId; // ID of the grandparent particle
    int grandParentPid; // PID of the grandparent particle
    Hadronium(int pid, int status, double px, double py, double pz, double e, std::vector<int> ids = {}, int parentId = -1, int parentPid = -1, int grandParentId = -1, int grandParentPid = -1);
};

Hadronium combine_particles(const std::vector<Hadronium>& particles);
std::vector<Hadronium> find_particles(const std::vector<Hadronium>& event, int pid);

template<typename T>
void get_combinations(const std::vector<T>& elements, int count, std::vector<std::vector<T>>& out_combinations);

std::vector<Hadronium> reconstruct_from_group(const std::vector<Hadronium>& event, const std::string& group);
std::vector<std::vector<Hadronium>> combine_lists(const std::vector<std::vector<Hadronium>>& x, const std::vector<std::vector<Hadronium>>& y);
bool has_shared_ids(const std::vector<Hadronium>& combination);
std::vector<std::vector<Hadronium>> filter_duplicate_combinations(const std::vector<std::vector<Hadronium>>& combinations);
std::vector<std::vector<Hadronium>> reconstruct_hadronia(LundEvent& event, const std::string& criteria, AcceptanceType acc);
std::vector<Hadronium> convertLundEventToHadronia(LundEvent& event, AcceptanceType acc);
void printHadronia(const std::vector<std::vector<Hadronium>>& hadroniums);

#endif // HADRONIUM_PARSER_H

