#ifndef HADRONIUM_FILTER_H
#define HADRONIUM_FILTER_H

#include <vector>
#include <functional>

#include "HadroniumParser.h"

struct ParticleCondition {
    int requiredParentPid = -1; // -1 means any parentPid is acceptable
    int requiredGrandParentPid = -1; // -1 means any grandParentPid is acceptable
};


enum class RelationshipType {
    SameParentId,
    ParentIdAsOtherGrandParentId,
    GrandParentIdAsOtherParentId,
    SameGrandParentId
};

struct ParentIdRelationship {
    size_t particleIndex1;
    size_t particleIndex2;
    std::vector<RelationshipType> types;

    ParentIdRelationship(size_t index1, size_t index2, const std::initializer_list<RelationshipType>& relTypes)
        : particleIndex1(index1), particleIndex2(index2), types(relTypes) {}
};

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
    
    bool isEmpty(){
        return (particleConditions.size()==0 && parentIdRelationships.size()==0);
    }
};

std::vector<std::vector<Hadronium>> filterHadronia(
    const std::vector<std::vector<Hadronium>>& hadronia, 
    const FilterRules& rules);

#endif // HADRONIUM_FILTER_H