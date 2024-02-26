#include "HadroniaFilter.h"

std::vector<std::vector<Hadronium>> filterHadronia(
    const std::vector<std::vector<Hadronium>>& hadronia, 
    const FilterRules& rules) {

    std::vector<std::vector<Hadronium>> filteredHadronia;

    for (const auto& hadronium : hadronia) {
        bool matchesConditions = true;
        
        // Check individual particle conditions
        for (size_t i = 0; i < hadronium.size(); ++i) {
            if (i < rules.particleConditions.size()) {
                const auto& condition = rules.particleConditions[i];
                const auto& particle = hadronium[i];

                if ((condition.requiredParentPid != -1 && particle.parentPid != condition.requiredParentPid) ||
                    (condition.requiredGrandParentPid != -1 && particle.grandParentPid != condition.requiredGrandParentPid)) {
                    matchesConditions = false;
                    break;
                }
            }
        }
        
        // Iterate over each relationship rule
        for (const auto& relationship : rules.parentIdRelationships) {
            // Ensure particle indices are within bounds
            if (relationship.particleIndex1 >= hadronium.size() || relationship.particleIndex2 >= hadronium.size()) {
                matchesConditions = false;
                break; // Skip this hadronium if indices are out of bounds
            }

            const auto& p1 = hadronium[relationship.particleIndex1];
            const auto& p2 = hadronium[relationship.particleIndex2];

            // Check each relationship type specified in the rule
            for (const auto& type : relationship.types) {
                switch (type) {
                    case RelationshipType::SameParentId:
                        if (p1.parentId != p2.parentId) matchesConditions = false;
                        break;
                    case RelationshipType::ParentIdAsOtherGrandParentId:
                        if (p1.parentId != p2.grandParentId) matchesConditions = false;
                        break;
                    case RelationshipType::GrandParentIdAsOtherParentId:
                        if (p1.grandParentId != p2.parentId) matchesConditions = false;
                        break;
                    case RelationshipType::SameGrandParentId:
                        if (p1.grandParentId != p2.grandParentId) matchesConditions = false;
                        break;
                }
                if (!matchesConditions) break; // Exit early if any condition fails
            }
            if (!matchesConditions) break; // Exit early if any rule fails
        }

        // Add the hadronium to the filtered list if it matches all conditions
        if (matchesConditions) {
            filteredHadronia.push_back(hadronium);
        }
    }

    return filteredHadronia;
}